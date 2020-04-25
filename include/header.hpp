// Copyright 2019 dimakirol <your_email>

#ifndef INCLUDE_HEADER_HPP_
#define INCLUDE_HEADER_HPP_

#include <vector>
#include <thread>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <mutex>
#include <ctime>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/expressions/keyword.hpp>

static const uint32_t SIZE_FILE = 10*1024*1024;
static const uint32_t Port = 2001;
static const uint32_t critical_time = 5;
static const uint32_t base_time = 100000000;
static const uint32_t additional_time = 300000000;

namespace assio = boost::asio;
using std::exception;
namespace logging = boost::log;

typedef boost::shared_ptr<assio::ip::tcp::socket> socket_ptr;

#endif // INCLUDE_HEADER_HPP_
