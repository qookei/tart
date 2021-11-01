#pragma once

#include <async/result.hpp>

[[gnu::weak]] void user_init();
async::result<void> async_main();
