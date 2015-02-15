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

        typedef typename std::pair<srcdiff_type, std::shared_ptr<T>> pair;

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

        const_iterator upper_bound(srcdiff_type operation) const {

            return entity.upper_bound(operation);

        }
        std::ostream & summarize_pure(std::ostream & out, srcdiff_type operation) const {

            size_t count = entity.count(operation);
            if(count == 0) return out;

            out << '\n';

           typename std::multimap<srcdiff_type, std::shared_ptr<T>>::const_iterator citr = entity.find(operation);

            const std::string type = is_function_type(citr->second->type_name) ? "function" : citr->second->type_name;
            profile_t::pad(out) << (operation == SRCDIFF_DELETE ? "Deleted " : "Inserted ") << type << "(s) (" << count << "): { ";
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

            typename std::multimap<srcdiff_type, std::shared_ptr<T>>::const_iterator citr = entity.find(SRCDIFF_COMMON);

            const std::string type = is_function_type(citr->second->type_name) ? "function" : citr->second->type_name;
            profile_t::pad(out) << "Modified " << type << "(s): " << num_modified << '\n';
            for(; citr != entity.upper_bound(SRCDIFF_COMMON); ++citr)
                citr->second->summary(out);

            return out;

        }

};

#endif