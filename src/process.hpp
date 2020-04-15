/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */
/* Copyright © 2011 Adam Cohen */

#pragma once
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace rapidfuzz { namespace process {
std::vector<std::pair<std::wstring, double> >
extract(const std::wstring& query, const std::vector<std::wstring>& choices,
    std::size_t limit = 5, double score_cutoff = 0, bool preprocess = true);

std::optional<std::pair<std::wstring, double> >
extractOne(const std::wstring& query, const std::vector<std::wstring>& choices,
    double score_cutoff = 0, bool preprocess = true);
} /* process */ } /* rapidfuzz */
