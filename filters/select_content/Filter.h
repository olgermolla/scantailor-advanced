/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C)  Joseph Artsimovich <joseph.artsimovich@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SELECT_CONTENT_FILTER_H_
#define SELECT_CONTENT_FILTER_H_

#include "NonCopyable.h"
#include "AbstractFilter.h"
#include "PageView.h"
#include "intrusive_ptr.h"
#include "FilterResult.h"
#include "SafeDeletingQObjectPtr.h"
#include "PageOrderOption.h"
#include "Settings.h"
#include <QCoreApplication>
#include <vector>

class PageId;
class PageSelectionAccessor;
class QString;

namespace page_layout {
    class Task;
    class CacheDrivenTask;
}

namespace select_content {
    class OptionsWidget;
    class Task;
    class CacheDrivenTask;
    class Settings;

    class Filter : public AbstractFilter {
    DECLARE_NON_COPYABLE(Filter)

    Q_DECLARE_TR_FUNCTIONS(select_content::Filter)
    public:
        explicit Filter(PageSelectionAccessor const& page_selection_accessor);

        ~Filter() override;

        QString getName() const override;

        PageView getView() const override;

        int selectedPageOrder() const override;

        void selectPageOrder(int option) override;

        virtual std::vector<PageOrderOption> pageOrderOptions() const;

        void performRelinking(AbstractRelinker const& relinker) override;

        void preUpdateUI(FilterUiInterface* ui, PageId const& page_id) override;

        void updateStatistics() override;

        QDomElement saveSettings(ProjectWriter const& writer, QDomDocument& doc) const override;

        void loadSettings(ProjectReader const& reader, QDomElement const& filters_el) override;

        void loadDefaultSettings(PageId const& page_id) override;

        intrusive_ptr<Task> createTask(PageId const& page_id,
                                       intrusive_ptr<page_layout::Task> next_task,
                                       bool batch,
                                       bool debug);

        intrusive_ptr<CacheDrivenTask>
        createCacheDrivenTask(intrusive_ptr<page_layout::CacheDrivenTask> next_task);

        OptionsWidget* optionsWidget();

    private:
        void writePageSettings(QDomDocument& doc, QDomElement& filter_el, PageId const& page_id, int numeric_id) const;


        intrusive_ptr<Settings> m_ptrSettings;
        SafeDeletingQObjectPtr<OptionsWidget> m_ptrOptionsWidget;
        std::vector<PageOrderOption> m_pageOrderOptions;
        int m_selectedPageOrder;
    };
}  // namespace select_content
#endif  // ifndef SELECT_CONTENT_FILTER_H_
