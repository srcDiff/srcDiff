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

            pad(out, depth);

            profile->print(out);

            out << "\n\n";
            for(size_t child_pos : profile->child_profiles)
                summarize(out, child_pos);

            return out;

        }

};

#endif
