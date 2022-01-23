#pragma once

namespace aoc
{
namespace science
{

class ProbeLauncher
{
public:
	ProbeLauncher& read_target(std::istream& is)
	{
		auto line = std::string{};
		std::getline(is, line);

		return *this;
	}

private:

};

}
}
