#ifndef INCLUDDED_SUMMARIZE_PROFILE
#define INCLUDDED_SUMMARIZE_PROFILE

#include <profile_t.hpp>
#include <unit_profile_t.hpp>
#include <function_profile_t.hpp>

class summarize_profile {

    private:

    protected:

        const profile_t::profile_list_t & profiles;

    public:

        summarize_profile(const profile_t::profile_list_t & profiles) : profiles(profiles) {}


        std::ostream & pad(std::ostream & out, size_t num) const {

            for(int i = 0; i < num; ++i)
                out << '\t';

            return out;

        }

        std::ostream & summarize(std::ostream & out, const size_t profile_pos) const {

            const std::shared_ptr<profile_t> & profile = profiles[profile_pos];

            if(profile->total_count == 0) return out;

            static int depth = 0;

            if(profile->syntax_count == 0) {

                if(profile->whitespace_count) pad(out, depth) << "Whitespace:\t" << profile->whitespace_count;
                if(profile->comment_count) pad(out, depth) << "Comment:\t" << profile->comment_count;

                out << '\n';

                return out;

            }

            pad(out, depth);

            try {


                if(profile->type_name == "unit")
                    out << *reinterpret_cast<const std::shared_ptr<unit_profile_t> &>(profile);
                else if(profile->type_name == "function")
                    out << *reinterpret_cast<const std::shared_ptr<function_profile_t> &>(profile);
                else
                    out << *profile;

            } catch(std::bad_cast cast) {fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);}

            if(!profile->inserted.empty()) {

                pad(out, depth + 1) << "--inserted--\n";

                for(std::pair<std::string, counts_t> entry : profile->inserted)
                    pad(out, depth + 1) << entry.first << ":\t" << entry.second.total << '\n';

            }

             if(!profile->deleted.empty()) {

                pad(out, depth + 1) << "--deleted--\n";

                for(std::pair<std::string, counts_t> entry : profile->deleted)
                    pad(out, depth + 1) << entry.first << ":\t" << entry.second.total << '\n';

            }

             if(!profile->modified.empty()) {

                pad(out, depth + 1) << "--modified--\n";

                for(std::pair<std::string, counts_t> entry : profile->modified)
                    pad(out, depth + 1) << entry.first << ":\t" << entry.second << '\n';

            }

            out << "\n\n";
            for(size_t child_pos : profile->child_profiles)
                summarize(out, child_pos);

            return out;

        }

};

#endif
