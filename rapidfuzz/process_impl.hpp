/* SPDX-License-Identifier: MIT */
/* Copyright © 2020 Max Bachmann */
/* Copyright © 2011 Adam Cohen */

namespace rapidfuzz {

template <typename Sentence1, typename CharT, typename Iterable, typename Sentence2,
          typename ProcessorFunc, typename ScorerFunc>
optional<std::pair<Sentence2, percent>>
process::extractOne(const Sentence1& query, const Iterable& choices, ProcessorFunc&& processor,
                    ScorerFunc&& scorer, const percent score_cutoff)
{
  static_assert(std::is_same<CharT, char_type<Sentence2>>::value,
    "When a processor is used query and choices have to use the same char type");

  bool match_found = false;
  percent best_score = score_cutoff;
  Sentence2 best_match;

  auto processed_query = processor(utils::to_string(query));

  for (const auto& choice : choices) {
    percent score = scorer(processed_query, processor(utils::to_string(choice)), best_score);

    if (score > best_score) {
      match_found = true;
      best_score = score;
      best_match = choice;
    }
  }

  if (!match_found) {
    return nullopt;
  }

  return std::make_pair(best_match, best_score);
}

template <typename Sentence1, typename Iterable, typename Sentence2, typename ScorerFunc>
optional<std::pair<Sentence2, percent>>
process::extractOne(const Sentence1& query, const Iterable& choices, nullopt_t,
                    ScorerFunc&& scorer, const percent score_cutoff)
{
  bool match_found = false;
  percent best_score = score_cutoff;
  Sentence2 best_match;

  for (const auto& choice : choices) {
    percent score = scorer(query, choice, best_score);
    if (score > best_score) {
      match_found = true;
      best_score = score;
      best_match = choice;
    }
  }

  if (!match_found) {
    return nullopt;
  }

  return std::make_pair(best_match, best_score);
}

template <typename Sentence1, typename CharT, typename Iterable, typename Sentence2,
          typename ProcessorFunc, typename ScorerFunc>
std::vector<std::pair<Sentence2, percent>>
process::extract(const Sentence1& query, const Iterable& choices, ProcessorFunc&& processor,
                 ScorerFunc&& scorer, const std::size_t limit, const percent score_cutoff)
{
  static_assert(std::is_same<CharT, char_type<Sentence2>>::value,
    "When a processor is used query and choices have to use the same char type");

  std::vector<std::pair<Sentence2, percent>> results;
  results.reserve(limit);
  // minimal score thats still in the result
  percent min_score = score_cutoff;

  auto processed_query = processor(utils::to_string(query));
  ;

  for (const auto& choice : choices) {
    percent score = scorer(processed_query, processor(utils::to_string(choice)), min_score);

    if (!utils::is_zero(score)) {
      if (results.size() > limit) {
        results.pop_back();
      }

      auto insert_pos =
          std::upper_bound(results.begin(), results.end(), score,
                           [](percent new_score, const std::pair<Sentence2, percent>& t2) {
                             return new_score > std::get<1>(t2);
                           });

      results.emplace(insert_pos, std::piecewise_construct, std::forward_as_tuple(choice),
                      std::forward_as_tuple(score));

      if (results.size() >= limit) {
        min_score = std::get<1>(results.back());
      }
    }
  }

  results.shrink_to_fit();

  return results;
}

template <typename Sentence1, typename Iterable, typename Sentence2, typename ScorerFunc>
std::vector<std::pair<Sentence2, percent>>
process::extract(const Sentence1& query, const Iterable& choices, nullopt_t,
                 ScorerFunc&& scorer, const std::size_t limit, const percent score_cutoff)
{
  std::vector<std::pair<Sentence2, percent>> results;
  results.reserve(limit);
  // minimal score thats still in the result
  percent min_score = score_cutoff;

  for (const auto& choice : choices) {
    percent score = scorer(query, choice, min_score);

    if (!utils::is_zero(score)) {
      if (results.size() == limit) {
        results.pop_back();
      }

      auto insert_pos =
          std::upper_bound(results.begin(), results.end(), score,
                           [](percent new_score, const std::pair<Sentence2, percent>& t2) {
                             return new_score > std::get<1>(t2);
                           });

      results.emplace(insert_pos, std::piecewise_construct, std::forward_as_tuple(choice),
                      std::forward_as_tuple(score));

      if (results.size() == limit) {
        min_score = std::get<1>(results.back());
      }
    }
  }

  results.shrink_to_fit();

  return results;
}

} // namespace rapidfuzz