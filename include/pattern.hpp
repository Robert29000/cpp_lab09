// Copyright 2020 Your Name <your_email>

#ifndef INCLUDE_PATTERN_HPP_
#define INCLUDE_PATTERN_HPP_

#include <string>
#include <utility>

bool correct_url(const std::string& url);

std::pair<std::string, std::string> divide_url(const std::string& url);

std::string download_page(const std::string& url);

#endif // INCLUDE_PATTERN_HPP_
