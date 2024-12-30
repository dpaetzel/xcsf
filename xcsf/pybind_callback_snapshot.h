/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file pybind_callback_snapshot.h
 * @author Richard Preen <rpreen@gmail.com>
 * @author David Pätzel
 * @copyright The Authors.
 * @date 2024.
 * @brief Snapshot callback for Python library.
 */

#pragma once

#include <limits>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <boost/filesystem.hpp>

namespace py = pybind11;
namespace fs = boost::filesystem;

extern "C" {
#include "xcsf.h"
}

#include "pybind_callback.h"
#include "pybind_utils.h"

/**
 * @brief Callback to make snapshots of XCSF at some frequency.
 */
class SnapshotCallback : public Callback
{
  public:
    /**
     * @brief Constructs a new snapshot callback.
     * @param [in] dirpath Path of the directory to save XCSF to.
     * @param [in] save_freq Trial frequency to make snapshots
     * @param [in] verbose Whether to display messages when an action is taken.
     */
    SnapshotCallback(std::string dirpath, int save_freq, bool verbose) :
        dirpath(dirpath),
        save_freq(save_freq),
        verbose(verbose)
    {
        std::ostringstream err;
        if (save_freq < 0) {
            err << "save_freq cannot be negative" << std::endl;
            throw std::invalid_argument(err.str());
        }
    }

    /**
     * @brief Saves the state of XCSF.
     * @param [in] xcsf The XCSF data structure.
     */
    void
    save(struct XCSF *xcsf)
    {
        if (!fs::exists(dirpath)) {
            fs::create_directory(dirpath);
        }
        std::string ntrials_str = std::to_string(xcsf->time);
        std::string filename = dirpath + "/" + ntrials_str + ".bin";
        xcsf_save(xcsf, filename.c_str());
        std::ostringstream status;
        status << get_timestamp() << " trials=" << ntrials_str;
        status << " SnapshotCallback: ";
        status << "saved " << filename;
        py::print(status.str());
    }

    /**
     * @brief Performs callback operations.
     * @param [in] xcsf The XCSF data structure.
     * @param [in] metrics Dictionary of performance metrics.
     * @return Whether to terminate training.
     */
    bool
    run(struct XCSF *xcsf, py::dict metrics) override
    {
        py::list trials = metrics["trials"];
        const int current_trial = py::cast<int>(trials[trials.size() - 1]);
        if (current_trial >= save_trial + save_freq) {
            save_trial = current_trial;
            save(xcsf);
        }
        return false;
    }

    /**
     * @brief Executes any tasks at the end of fitting.
     * @param [in] xcsf The XCSF data structure.
     */
    void
    finish(struct XCSF *xcsf) override
    {
    }

  private:
    std::string dirpath; //!< Path of the directory to save XCSF to
    int save_freq; //!< Trial frequency to (possibly) make snapshots
    bool verbose; //!< Whether to display messages when an action is taken

    int save_trial = 0; //!< Trial number the last snapshot was made
};
