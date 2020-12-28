// Copyright 2020 Your Name <your_email>

#include <gtest/gtest.h>
#include <pattern.hpp>

TEST(Pattern, Correct_url) {
    std::string t = "http://yandex.ru";
    EXPECT_TRUE(correct_url(t));
    std::string t1 = "https://mail.ru/work/";
    EXPECT_TRUE(correct_url(t1));
    std::string t2 = "http://127.0.0.1";
    EXPECT_TRUE(correct_url(t2));
    std::string t3 = "http:/vk.com";
    EXPECT_FALSE(correct_url(t3));
    std::string t4 = "ua.ru";
    EXPECT_FALSE(correct_url(t4));
}

TEST(Pattern, divide_url){
  std::string url = "http://kremlin.ru";
  auto p = divide_url(url);
  EXPECT_EQ(p.first, "kremlin.ru");
  EXPECT_EQ(p.second, "/");
  std::string url1 = "https://vk.com/home/index";
  auto p1 = divide_url(url1);
  EXPECT_EQ(p1.first, "vk.com");
  EXPECT_EQ(p1.second, "/home/index");
  std::string url2 = "http://127.0.0.1/config/";
  auto p2 = divide_url(url2);
  EXPECT_EQ(p2.first, "127.0.0.1");
  EXPECT_EQ(p2.second, "/config/");
}
