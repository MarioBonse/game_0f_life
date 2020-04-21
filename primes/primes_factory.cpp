/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ***************************************************************************
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  As a special exception, you may use this file as part of a free software
 *  library without restriction.  Specifically, if other files instantiate
 *  templates or use macros or inline functions from this file, or you compile
 *  this file and link it with other files to produce an executable, this
 *  file does not by itself cause the resulting executable to be covered by
 *  the GNU General Public License.  This exception does not however
 *  invalidate any other reasons why the executable file might be covered by
 *  the GNU General Public License.
 *
 ****************************************************************************
 */
/* 
 * Author: Massimo Torquati <torquati@di.unipi.it> 
 * Date:   October 2014
 */
/* This program prints to the STDOUT all prime numbers 
 * in the range (n1,n2) where n1 and n2 are command line 
 * parameters.
 *
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ff/utils.hpp>
#include <ff/ff.hpp>
#include <ff/parallel_for.hpp>
#include <ff/farm.hpp>
#include <mutex>

using namespace ff;
using ull = unsigned long long;
// see http://en.wikipedia.org/wiki/Primality_test
static bool is_prime(ull n) {
    if (n <= 3)  return n > 1; // 1 is not prime !
    
    if (n % 2 == 0 || n % 3 == 0) return false;

    for (ull i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) 
            return false;
    }
    return true;
}

struct range{
    range(ull start, ull end):start(start),end(end){}
    ull start;
    ull end;
    std::vector<ull> result ={};
};

struct master: ff_node_t<range> {

    master(const ull start, const ull end, const int nw, bool print_primes):start(start),end(end),nw(nw),print_primes(print_primes) {}

    range* svc(range* computed) {
        if (computed == nullptr) {
            ull step = (end - start)/nw;
            ull new_start = start;
            std::vector<ull> *res;
            for(; new_start<end; new_start += step) {
                ff_send_out(new range(new_start, std::min(end, new_start + step)));
            }
            return GO_ON;
        }        
        //concat the two results
        results.insert(results.end(), computed->result.begin(), computed->result.end());

        delete computed;
        iteration ++;
        if(iteration==nw)
            return EOS;
        return GO_ON;         
    }
    void svc_end() { 
        int n = results.size();
        std::cout << "Found " << n << " primes\n"; 
        if (print_primes) {
            for(size_t i=0;i<n;++i) 
                std::cout << results[i] << " ";
            std::cout << "\n\n";
        }

    }

    ull start;
    ull end;
    int nw;
    bool print_primes;
    std::vector<ull> results = {};
    int iteration = 0;
};

struct worker: ff_node_t<struct range> {
    range* svc(struct range * r) { 
        std::vector<ull> &result = r->result;
        for(auto i = r->start; i< r->end; i++){
            if(is_prime(i)){
                result.push_back(i);
            }
        }
        ff_send_out(r);
        return GO_ON; 
    }
};

int main(int argc, char *argv[]) {    
    if (argc<4) {
        std::cerr << "use: " << argv[0]  << " number1 number2 [print=off|on] nw\n";
        return -1;
    }
    ull n1          = std::stoll(argv[1]);
    ull n2          = std::stoll(argv[2]);  
    bool print_primes = false;
    if (argc >= 4)  print_primes = (std::string(argv[3]) == "on");

    int nw = atoi(argv[4]);

    std::mutex list_mutex;
    ff::ffTime(ff::START_TIME);
    // set up the farm
    // create the workers
    master M(n1, n2, nw, print_primes);
    std::vector<std::unique_ptr<ff_node>> W;
    for(int i = 0; i < nw; i++)W.push_back(make_unique<worker>());
    // create the farm and remove the collector
    ff_Farm<std::vector<ull>> farm(std::move(W), M);
    farm.remove_collector();
    farm.wrap_around();

    if (farm.run_and_wait_end()<0) {
        error("running farm");
        return -1;
    }
    ff::ffTime(ff::STOP_TIME);
    std::cout << "Time: " << ff::ffTime(ff::GET_TIME) << " (ms)\n";
    return 0;
}