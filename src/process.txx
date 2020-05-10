#include "process.hpp"

namespace rapidfuzz {


template<
    typename Sentence1, typename CharT,
    typename Iterable, typename Sentence2,
    typename ProcessorFunc, typename ScorerFunc,
    typename
>
boost::optional<std::pair<Sentence2, percent>> process::extractOne(
    const Sentence1& query,
    const Iterable& choices,
    ProcessorFunc&& processor,
    ScorerFunc&& scorer,
    const percent score_cutoff
)
{
	bool match_found = false;
	percent best_score = score_cutoff;
    Sentence2 best_match;

	auto processed_query = processor(std::basic_string<CharT>(query));

	for (const auto& choice : choices) {
		percent score = scorer(
            processed_query,
            processor(std::basic_string<CharT>(choice)),
            best_score);

		if (score > best_score) {
			match_found = true;
			best_score = score;
			best_match = choice;
		}
	}

	if (!match_found) {
		return boost::none;
	}

	return std::make_pair(best_match, best_score);
}


template<
    typename Sentence1, typename CharT,
    typename Iterable, typename Sentence2,
    typename ScorerFunc,
	typename
>
boost::optional<std::pair<Sentence2, percent>> process::extractOne(
    const Sentence1& query,
    const Iterable& choices,
    boost::none_t,
    ScorerFunc&& scorer,
    const percent score_cutoff
)
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
		return boost::none;
	}

	return std::make_pair(best_match, best_score);
}

template<
    typename Sentence1, typename CharT,
    typename Iterable, typename Sentence2,
    typename ProcessorFunc, typename ScorerFunc,
    typename
>
std::vector<std::pair<Sentence2, percent>> process::extract(
    const Sentence1& query,
    const Iterable& choices,
    ProcessorFunc&& processor,
    ScorerFunc&& scorer,
    const std::size_t limit,
    const percent score_cutoff
)
{
    std::vector<std::pair<Sentence2, percent>> results;
    results.reserve(limit);
    // minimal score thats still in the result
    percent min_score = score_cutoff;

    auto processed_query = processor(std::basic_string<CharT>(query));;

    for (const auto& choice : choices) {
        percent score = scorer(
            processed_query,
            processor(std::basic_string<CharT>(choice)),
            min_score);

        if (!utils::is_zero(score)) {
            if (results.size() > limit) {
              results.pop_back();
            }

            auto insert_pos = std::upper_bound(
                results.begin(), results.end(),
                score,
                [](percent new_score, auto const& t2) {
                    return new_score > std::get<1>(t2);
                }
            );

            results.emplace( 
                insert_pos,
                std::piecewise_construct,
                std::forward_as_tuple(choice),
                std::forward_as_tuple(score)
            );
            
            if (results.size() >= limit) {
              min_score = std::get<1>(results.back());
            }
        }
    }

    results.shrink_to_fit();

    return results;
}

template<
    typename Sentence1, typename CharT,
    typename Iterable, typename Sentence2,
    typename ScorerFunc,
    typename
>
std::vector<std::pair<Sentence2, percent>> process::extract(
    const Sentence1& query,
    const Iterable& choices,
    boost::none_t,
    ScorerFunc&& scorer,
    const std::size_t limit,
    const percent score_cutoff
)
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

            auto insert_pos = std::upper_bound(
                results.begin(), results.end(),
                score,
                [](percent new_score, auto const& t2) {
                    return new_score > std::get<1>(t2);
                }
            );

            results.emplace( 
                insert_pos,
                std::piecewise_construct,
                std::forward_as_tuple(choice),
                std::forward_as_tuple(score)
            );
            
            if (results.size() == limit) {
              min_score = std::get<1>(results.back());
            }
        }
    }

    results.shrink_to_fit();

    return results;
}


} /* rapidfuzz */