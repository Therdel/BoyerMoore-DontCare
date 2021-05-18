template< class RandomIt2 >
auto BoyerMoore::operator()(RandomIt2 first, RandomIt2 last) const->std::pair<RandomIt2, RandomIt2>
{
	const uint8_t* rawFirst = reinterpret_cast<const uint8_t*>(&*first);
	auto length = last - first;

	auto haystack = std::basic_string_view<uint8_t>(rawFirst, length);
	auto result = (*this)(haystack);

	ptrdiff_t firstDist = result.first - rawFirst;
	ptrdiff_t lastDist = result.second - rawFirst;
	return { first + firstDist, first + lastDist };
}