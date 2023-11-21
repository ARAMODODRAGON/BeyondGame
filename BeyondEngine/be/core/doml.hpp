#ifndef BOE_DOML_HPP
#define BOE_DOML_HPP
#include <string>

namespace be {

	// basic data serialization 
	class doml final {
	public:

		doml(std::nullptr_t = nullptr);

		doml(const std::string& filepath);


	};

}

#endif // !BOE_DOML_HPP