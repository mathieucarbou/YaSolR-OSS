// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2023-2024 Mathieu Carbou
 */
#include <YaSolR.h>
#include <YaSolRWebsite.h>

Mycila::Task initDashboardCards("Init Dashboard", [](void* params) {
  logger.info(TAG, "Initializing dashboard");
  YaSolR::Website.initLayout();
  YaSolR::Website.initCards();
  YaSolR::Website.updateCards();
});
