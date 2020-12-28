#include <ThreadPool.h>
#include <gumbo.h>
#include <algorithm>
#include <boost/program_options.hpp>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
#include <set>
#include <string>
#include <vector>
#include <pattern.hpp>


using namespace boost::program_options;

std::mutex m_;
std::set<std::string> urls;



std::string parse_links(GumboNode* node){
  if (node->type != GUMBO_NODE_ELEMENT) {
    return "";
  }
  GumboAttribute* href;
  if (node->v.element.tag == GUMBO_TAG_A &&
      (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
    if (correct_url(href->value)){
      std::lock_guard lock(m_);
      urls.insert(href->value);
    }
  }
  std::string res;
  GumboAttribute* src;
  if (node->v.element.tag == GUMBO_TAG_IMG && (src = gumbo_get_attribute(&node->v.element.attributes, "src"))){
    if (correct_url(src->value)) {
      res = std::string(src->value) + '\n';
    }
  }

  GumboVector* children = &node->v.element.children;
  for (unsigned int i = 0; i < children->length; ++i) {
    res += parse_links(static_cast<GumboNode*>(children->data[i]));
  }
  return res;
}

std::string parse_page(const std::string& body){
  GumboOutput* output = gumbo_parse(body.c_str());
  auto res = parse_links(output->root);
  gumbo_destroy_output(&kGumboDefaultOptions, output);
  return res;
}

int main(int argc, char* argv[]) {
  try {
    int depth, net_threads, par_threads;
    std::string output_p;
    options_description desc{"Allowed options"};
    desc.add_options()("help,h", "Show help")
        ("url,u", value<std::string>(),"set url for html page to parse")   // for example http://kremlin.ru
            ("depth,d", value<int>(&depth)->default_value(1), "set depth of parse")
                ("network_threads,n", value<int>(&net_threads)->default_value(2),"set number of threads to download pages")
                    ("parser_threads,p", value<int>(&par_threads)->default_value(2), "set number of threads to parse pages")
                        ("output,o", value<std::string>(&output_p)->default_value(""), "set output directory");
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    std::ofstream file(output_p + "output.txt");

    urls.insert(vm["url"].as<std::string>());
    for (auto i = 1; i <= depth; i++){
      ThreadPool download_pool(net_threads);
      ThreadPool parse_pool(par_threads);
      std::vector<std::future<std::string>> pages;
      std::vector<std::future<std::string>> parsed;
      auto temp_urls = std::move(urls);
      urls.clear();
      for (const auto& url: temp_urls) {
        pages.push_back(download_pool.enqueue(
            &download_page, std::ref(url)));
      }
      for (auto& el:pages){
        parsed.push_back(parse_pool.enqueue(
            &parse_page, el.get()));
      }
      for (auto& el: parsed){
        auto res = el.get();
        if (!res.empty()) {
          file << res;
        }
      }
    }
    file.close();
  } catch(const error& ex){
    std::cerr << ex.what() << '\n';
  }
}