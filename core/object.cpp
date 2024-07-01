#include "object.h"

namespace sgd {

void debugObjects(bool diffsOnly) {
	
	static Map<ObjectType*, int> counts;
	static String pad = "                   ";
	
	for (auto type = ObjectType::allTypes(); type; type = type->succ) {
		int count = type->instanceCount();
		int diff = count - counts[type];
		counts[type] = count;
		if (diffsOnly && !diff) continue;
		String dstr = diff ? (String(diff > 0 ? "(+" : "(") + std::to_string(diff) + ')') : "";
		String name = (String(type->name) + pad).substr(0, pad.size());
		SGD_LOG << name << count << dstr;
	}
	SGD_LOG << "---------------------------------";
}

}
