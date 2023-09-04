#include "../utils.hpp"
#include "ioh/problem/cec.hpp"
#include "ioh/problem/cec/cec_problem.hpp"

TEST_F(BaseTest, CECProblem)
{
    const auto file_name = ioh::common::file::utils::find_static_file("cec_problem.in");
    std::string s;

    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::CEC>::instance();

    std::ifstream infile(file_name.c_str());
    ASSERT_TRUE(infile.is_open());
    while (getline(infile, s))
    {
        auto tmp = split(s, " ");
        if (tmp.empty()) { continue; }

        auto func_id = stoi(tmp[0]);
        auto ins_id = stoi(tmp[1]);
        auto x = string_to_vector_double(tmp[2]);
        auto f = stod(tmp[3]);

        auto problem = problem_factory.create(func_id, ins_id, x.size());
        auto y = (*problem)(x);
        EXPECT_LE(abs(y - f) / f, 1.0 / pow(10, 6 - log(10)))
            << "The fitness of function " << func_id << "( ins "
            << ins_id << " ) is " << f << " ( not " << y << ").";
    }
}

TEST_F(BaseTest, xopt_equals_yopt_cec)
{
    const auto& problem_factory = ioh::problem::ProblemRegistry<ioh::problem::CEC>::instance();
    for (const auto& name : problem_factory.names())
    {
        auto problem = problem_factory.create(name, 1, 10);

        // Calling this from the constructor results in an SIGSEGV, because the object has not been fully initialized yet.
        // And there is no other appropriate place.
        problem->set_optimum();

        EXPECT_DOUBLE_EQ(problem->optimum().y, (*problem)(problem->optimum().x)) << *problem;
    }
}
