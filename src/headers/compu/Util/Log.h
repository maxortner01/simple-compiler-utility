#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Singleton.h"

namespace compu
{
namespace Util
{
    struct Log : public Singleton<Log>
    {
        spdlog::logger* log;
        inline void init();
    };
}
}

void compu::Util::Log::init()
{
    log = spdlog::stdout_color_mt("Compu").get();
    log->set_pattern("%^[%T] %n: %v%$");
    log->set_level(spdlog::level::trace);
}

#define COMP_INFO(...) compu::Util::Log::get().log->info(__VA_ARGS__)
#define COMP_WARN(...) compu::Util::Log::get().log->warn(__VA_ARGS__)
#define COMP_ERROR(...) compu::Util::Log::get().log->error(__VA_ARGS__)
#define COMP_TRACE(...) compu::Util::Log::get().log->trace(__VA_ARGS__)
#define COMP_DEBUG(...) compu::Util::Log::get().log->debug(__VA_ARGS__)
