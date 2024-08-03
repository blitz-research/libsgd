#include "modelrenderer.h"

#include "model.h"

namespace sgd {

namespace {}

ModelRenderer::ModelRenderer()
	:										  //
	  m_opaqueRenderer(new MeshRenderer()),	  //
	  m_blendedRenderer(new MeshRenderer()) { //
}

void ModelRenderer::add(CModel* model) {

	auto mesh = model->mesh();
	if (!mesh) {
		model->mesh.changed.connect(this, [=](CMesh*) { //
			model->mesh.changed.disconnect(this);
			add(model);
		});
		return;
	}

	// Always add to opaque list for now
	auto it = m_opaqueLists.find(mesh);
	if (it == m_opaqueLists.end()) {
		it = m_opaqueLists.insert(std::make_pair(mesh, new OpaqueInstanceList())).first;
	}
	auto opaqueList = it->second.get();
	opaqueList->models.push_back(model);
	++m_numOpaqueInsts;

	if (!bool(mesh->flags() & MeshFlags::blendedSurfaces)) {
		model->mesh.changed.connect(this, [=](CMesh*) {
			model->mesh.changed.disconnect(this);
			sgd::remove(opaqueList->models, model); // remove from opaque list only...
			--m_numOpaqueInsts;
			add(model);
		});
	}

	// Add to blended list
	m_blendedInsts.emplace_back(model);
	++m_numBlendedInsts;

	model->mesh.changed.connect(this, [=](CMesh*) {
		model->mesh.changed.disconnect(this);
		// Remove from opaque list
		sgd::remove(opaqueList->models, model);
		--m_numOpaqueInsts;
		// Remove from blended list
		for (auto it = m_blendedInsts.begin(); it != m_blendedInsts.end(); ++it) {
			if (it->model != model) continue;
			m_blendedInsts.erase(it);
			--m_numBlendedInsts;
			add(model);
			return;
		}
		SGD_ABORT();
	});
}

void ModelRenderer::remove(CModel* model) {

	model->mesh.changed.disconnect(this);

	auto mesh = model->mesh();
	if (!mesh) return;

	// Remove from opaque list
	auto opaqueList = m_opaqueLists.find(mesh)->second.get();
	sgd::remove(opaqueList->models, model);
	--m_numOpaqueInsts;

	if (!bool(mesh->flags() & MeshFlags::blendedSurfaces)) return;

	// Remove from blended list
	for (auto it = m_blendedInsts.begin(); it != m_blendedInsts.end(); ++it) {
		if (it->model != model) continue;
		m_blendedInsts.erase(it);
		--m_numBlendedInsts;
		return;
	}
	SGD_ABORT();
}

void ModelRenderer::update(CVec3r eye) {

	// Update opaque instances
	if (!m_opaqueLists.empty()) {

		auto instp = m_opaqueRenderer->lockInstances(m_numOpaqueInsts);

		for (auto& kv : m_opaqueLists) {
			auto list = kv.second.get();
			for (CModel* model : list->models) {
				instp->worldMatrix = model->worldMatrix();
				instp->worldMatrix.t.xyz() -= eye;
				instp->color = model->pmColor();
				++instp;
			}
		}
		m_opaqueRenderer->unlockInstances();
	}

	if (!m_blendedInsts.empty()) {

		for (auto& inst : m_blendedInsts) {
			inst.distance = (float)length(inst.model->worldMatrix() * Vec3r(inst.model->mesh()->origin()) - eye);
		}

		std::sort(m_blendedInsts.begin(), m_blendedInsts.end(), [](const BlendedInstance& lhs, const BlendedInstance& rhs) { //
			return lhs.distance > rhs.distance;
		});

		m_blendedSurfs.clear();

		auto instp = m_blendedRenderer->lockInstances(m_blendedInsts.size());

		for (auto& inst : m_blendedInsts) {

			instp->worldMatrix = inst.model->worldMatrix();
			instp->worldMatrix.t.xyz() -= eye;
			instp->color = inst.model->pmColor();
			++instp;

			m_surfaceInsts.clear();
			for (CSurface* surf : inst.model->mesh()->surfaces()) {
				if (surf->material()->blendMode() != BlendMode::alphaBlend) continue;
				float distance = (float)length(inst.model->worldMatrix() * Vec3r(surf->origin()) - eye);
				m_surfaceInsts.emplace_back(surf, distance);
			}

			// Sort surfaces within model instance bounds only
			std::sort(m_surfaceInsts.begin(), m_surfaceInsts.end(),
					  [](const SurfaceInstance& lhs, const SurfaceInstance& rhs) { //
						  return lhs.distance > rhs.distance;
					  });

			for (auto& surf : m_surfaceInsts) {
				m_blendedSurfs.push_back(surf.surface);
			}
			m_blendedSurfs.push_back(nullptr);
		}
		m_blendedRenderer->unlockInstances();
	}
}

void ModelRenderer::render(RenderQueue* rq) const {

	//  Render opaque
	{
		uint32_t first = 0;
		for (auto& kv : m_opaqueLists) {
			auto mesh = kv.first;
			uint32_t count = kv.second->models.size();
			if (bool(mesh->flags() & MeshFlags::blendedSurfaces)) {
				for (Surface* surf : mesh->surfaces()) {
					if (surf->material()->blendMode() == BlendMode::alphaBlend) continue;
					m_opaqueRenderer->render(rq, surf, count, first);
				}
			} else {
				m_opaqueRenderer->render(rq, mesh, count, first);
			}
			first += count;
		}
	}

	if (m_blendedSurfs.empty()) return;

	// Render blended
	{
		uint32_t index = 0;
		for (auto surf : m_blendedSurfs) {
			if (surf) {
				m_blendedRenderer->render(rq, surf, 1, index);
			} else {
				++index;
			}
		}
	}
}

} // namespace sgd
