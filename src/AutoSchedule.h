#ifndef HALIDE_INTERNAL_AUTO_SCHEDULE_H
#define HALIDE_INTERNAL_AUTO_SCHEDULE_H

/** \file
 *
 * Defines the method that does automatic scheduling of Funcs within a pipeline.
 */

#include "Function.h"
#include "Target.h"

namespace Halide {

/** A struct representing the machine parameters to generate the auto-scheduled
 * code for. */
struct MachineParams {
    /** Maximum level of parallelism avalaible. */
    Expr parallelism;
    /** Size of the last-level cache (in KB). */
    Expr last_level_cache_size;
    /** Indicates how much more expensive is the cost of a load compared to
     * the cost of an arithmetic operation at last level cache. */
    Expr balance;

    explicit MachineParams(int32_t parallelism, int32_t llc, int32_t balance)
        : parallelism(parallelism), last_level_cache_size(llc), balance(balance) {}

    /** Default machine parameters for generic CPU architecture. */
    static MachineParams generic();

    /** Convert the MachineParams into canonical string form. */
    std::string to_string() const;

    /** Reconstruct a MachineParams from canonical string form. */
    explicit MachineParams(const std::string &s);
};

namespace Internal {

/** If the cost of computing a Func is about the same as calling the Func,
 *  inline the Func. Return true of any of the Funcs is inlined. */
bool inline_all_trivial_functions(const std::vector<Function> &outputs,
                                  const std::vector<std::string> &order,
                                  const std::map<std::string, Function> &env);

/** Inline a Func if its values are only consumed by another single Func in
 * element-wise manner. */
bool inline_all_element_wise_functions(const std::vector<Function> &outputs,
                                       const std::vector<std::string> &order,
                                       const std::map<std::string, Function> &env);

/** Check if all the pipeline outputs have estimates specified
 *  on each of their dimensions; otherwise, throw an assertion. */
void check_estimates_on_outputs(const std::vector<Function> &outputs);

/** Generate schedules for Funcs within a pipeline. The Funcs should not already
 * have specializations or schedules as the current auto-scheduler does not take
 * into account user-defined schedules or specializations. This applies the
 * schedules and returns a string representation of the schedules. The target
 * architecture is specified by 'target'. */
std::string generate_schedules(const std::vector<Function> &outputs,
                               const Target &target,
                               const MachineParams &arch_params);

}  // namespace Internal
}  // namespace Halide

#endif
