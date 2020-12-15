#include <Config/LocationList.hpp>
namespace Config
{
LocationList::LocationList() {}
LocationList::LocationList(LocationList const &x) : location_(x.location_) {}
LocationList &LocationList::operator=(LocationList const &x)
{
	location_ = x.location_;
	return *this;
}
LocationList::~LocationList() {}

Location const &LocationList::match(std::string const &target) const
{
	int regex = -1;
	std::pair<size_t, size_t> match(0, 0);
	size_t n;

	for (size_t i = 0; i < location_.size(); ++i)
	{
		if (location_[i].exactMatch(target))
			return location_[i];
#ifdef BONUS
		else if (regex < 0 && location_[i].regexMatch(target))
			regex = i;
#endif
		else if ((n = location_[i].matches(target)) && n > match.first)
			match = std::make_pair(n, i);
	}
	return (regex < 0) ? location_[match.second] : location_[regex];
}

Location &LocationList::add(Location toAdd)
{
	location_.push_back(toAdd);
	return location_.back();
}

bool LocationList::empty()
{
	return location_.empty();
}

void LocationList::print()
{
	for (std::vector<Location>::iterator it = location_.begin(); it != location_.end(); ++it)
		it->print();
}
}  // namespace Config
