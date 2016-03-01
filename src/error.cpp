#include "helper.hpp"
#include <boost/assign/list_of.hpp>
#include <unordered_map>
namespace glrfw {

namespace {

const std::unordered_map<error_type, const char*> error_map =
	boost::assign::map_list_of(shader_already_created,"shader_already_created")
	(shader_already_compiled,"shader_already_compiled")
	(shader_not_compiled,"shader_not_compiled")
	(shader_not_created,"shader_not_created")
	(program_already_created,"program_already_created")
	(program_already_linked,"program_already_linked")
	(program_not_linked,"program_not_linked")
	(program_not_created,"program_not_created")
	(no_shader_source,"no_shader_source")
	(file_not_found,"file_not_found")
	(uniform_not_found,"uniform_not_found")
    (invalid_shader_type,"invalid_shader_type");

} // end of anonymous namespace

const char* errors_to_str(error_type type)
{
	return error_map[type];

}


} // end of glrfw namespace 
