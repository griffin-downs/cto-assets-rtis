// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

namespace ctoAssetsRTIS
{
template<typename ResultType, typename StringProvider>
class CompileTimeDeserialize;

template<typename ResultType, typename StringProvider>
auto deserialize()
{
    return
        ResultType(
            CompileTimeDeserialize<ResultType, StringProvider>::value);
}
} // namespace ctoAssetsRTIS
