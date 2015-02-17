#ifndef INCLUDED_CHANGE_ENTITY_MAP_HPP
#define INCLUDED_CHANGE_ENTITY_MAP_HPP

#include <type_query.hpp>
#include <profile_t.hpp>

#include <map>
#include <memory>

template<typename T>
class change_entity_map {

	private:

	protected:

		std::multimap<srcdiff_type, std::shared_ptr<T>> entity;

	public:

        typedef typename std::multimap<srcdiff_type, std::shared_ptr<T>>::iterator       iterator;
        typedef typename std::multimap<srcdiff_type, std::shared_ptr<T>>::const_iterator const_iterator;
		typedef typename std::multimap<srcdiff_type, std::shared_ptr<T>>::size_type      size_type;
        typedef typename std::pair<srcdiff_type, std::shared_ptr<T>>                     pair;

		change_entity_map() {}

		size_type count(srcdiff_type operation) const {

			return entity.count(operation);

		}

		template<class ... Args>
		iterator emplace(Args &&... args) {

			return entity.template emplace<Args...>(args...);

		}

        const_iterator find(srcdiff_type operation) const {

            return entity.find(operation);

        }

        const_iterator end() const {

            return entity.end();

        }

        const_iterator lower_bound(srcdiff_type operation) const {

            return entity.lower_bound(operation);

        }

        const_iterator upper_bound(srcdiff_type operation) const {

            return entity.upper_bound(operation);

        }

        static const std::string type_category(const std::string & type_name) {

            if(is_decl_stmt(type_name))      return "decl_stmt";
            if(is_parameter(type_name))      return "parameter";
            if(is_function_type(type_name))  return "function";
            if(is_class_type(type_name))     return "class";
            if(is_condition_type(type_name)) return "conditional";
            return type_name;
     
        }

        static const std::string type_category(const versioned_string & type_name) {

            if(type_name.is_common()) return type_category(type_name.original());

            const std::string type_original = type_category(type_name.original());
            const std::string type_modified = type_category(type_name.modified());

            assert(type_original == type_modified);

            return type_original;

        }

        std::ostream & summarize_pure(std::ostream & out, srcdiff_type operation) const {

            size_t count = entity.count(operation);
            if(count == 0) return out;

            out << '\n';

            typename std::multimap<srcdiff_type, std::shared_ptr<T>>::const_iterator citr = entity.lower_bound(operation);

            profile_t::pad(out) << (operation == SRCDIFF_DELETE ? "Deleted " : "Inserted ") << type_category(citr->second->type_name) << "(s) (" << count << "): { ";
            citr->second->summary(out);
            ++citr;
            for(; citr != entity.upper_bound(operation); ++citr) {

                out << ", ";
                citr->second->summary(out);

            }

            out << " }\n";

            return out;

        }

        std::ostream & summarize_modified(std::ostream & out) const {

            size_t num_modified = entity.count(SRCDIFF_COMMON);
            if(num_modified == 0) return out;

            out << '\n';

            typename std::multimap<srcdiff_type, std::shared_ptr<T>>::const_iterator citr = entity.lower_bound(SRCDIFF_COMMON);

            profile_t::pad(out) << "Modified " << type_category(citr->second->type_name) << "(s): " << num_modified << '\n';
            for(; citr != entity.upper_bound(SRCDIFF_COMMON); ++citr)
                if(citr->second->syntax_count)
                        citr->second->summary(out);

            return out;

        }

};

#endif
