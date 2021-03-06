// Copyright (C) 2019  Joseph Artsimovich <joseph.artsimovich@gmail.com>, 4lex4 <4lex49@zoho.com>
// Use of this source code is governed by the GNU GPLv3 license that can be found in the LICENSE file.

#ifndef SCANTAILOR_FIX_ORIENTATION_CACHEDRIVENTASK_H_
#define SCANTAILOR_FIX_ORIENTATION_CACHEDRIVENTASK_H_

#include <memory>

#include "CompositeCacheDrivenTask.h"
#include "NonCopyable.h"

class PageInfo;
class AbstractFilterDataCollector;

namespace page_split {
class CacheDrivenTask;
}

namespace fix_orientation {
class Settings;

class CacheDrivenTask : public CompositeCacheDrivenTask {
  DECLARE_NON_COPYABLE(CacheDrivenTask)

 public:
  CacheDrivenTask(std::shared_ptr<Settings> settings, std::shared_ptr<page_split::CacheDrivenTask> nextTask);

  virtual ~CacheDrivenTask();

  void process(const PageInfo& pageInfo, AbstractFilterDataCollector* collector) override;

 private:
  std::shared_ptr<page_split::CacheDrivenTask> m_nextTask;
  std::shared_ptr<Settings> m_settings;
};
}  // namespace fix_orientation
#endif  // ifndef SCANTAILOR_FIX_ORIENTATION_CACHEDRIVENTASK_H_
