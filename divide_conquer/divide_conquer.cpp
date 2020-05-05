#include <vector>
#include <thread>

template<typename Tin, typename Tout>
Tout sequential_dc(Tin input,
        bool (*basecase)(Tin),
        Tout (*solve)(Tin),
        std::vector<Tin> (*divide)(Tin),
        Tout (*conquer)(std::vector<Tout>)) {

    if(basecase(input)) {
        return(solve(input));
    } else {
        auto subproblems = divide(input);

        std::transform(subproblems.begin(),
        subproblems.end(),
        subproblems.begin(),
        [&](Tin x) {
        auto res = sequential_dc(x, basecase, solve, divide, conquer);
        return(res);
        });

        auto result = conquer(subproblems);
        return(result);
    }
};

template<typename Tin, typename Tout>
Tout parallel_dc(Tin input,
        bool (*basecase)(Tin),
        Tout (*solve)(Tin),
        std::vector<Tin> (*divide)(Tin),
        Tout (*conquer)(std::vector<Tout>)) {

    if(basecase(input)) {
        return(solve(input));
    } else {
        auto subproblems = divide(input);
        std::vector<Tout> subresults(subproblems.size());

        auto thread_process = [&subresults, basecase, solve, divide, conquer, &subproblems](int i){
            subresults[i] = sequential_dc(subproblems[i], basecase, solve, divide, conquer);
        };



        // start a thread for each subproblem and wait 

        // list of threads
        std::vector<std::thread> tids;

        for(auto i = 0; i != subproblems.size(); i++){
            tids.push_back(std::thread(thread_process, i));
        }

        for(std::thread& t: tids) {                        // await thread termination
            t.join();
        }

        auto result = conquer(subresults);
        return(result);
    }
};

