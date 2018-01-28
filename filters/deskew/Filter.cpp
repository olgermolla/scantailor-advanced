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

#include "Filter.h"
#include "FilterUiInterface.h"
#include "OptionsWidget.h"
#include "Task.h"
#include "CacheDrivenTask.h"
#include "ProjectReader.h"
#include "ProjectWriter.h"
#include "AbstractRelinker.h"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <utility>
#include <DefaultParams.h>
#include <DefaultParamsProvider.h>
#include <CommandLine.h>
#include <filters/select_content/Task.h>
#include <filters/select_content/CacheDrivenTask.h>

namespace deskew {
    Filter::Filter(PageSelectionAccessor const& page_selection_accessor)
            : m_ptrSettings(new Settings) {
        if (CommandLine::get().isGui()) {
            m_ptrOptionsWidget.reset(new OptionsWidget(m_ptrSettings, page_selection_accessor));
        }
    }

    Filter::~Filter() = default;

    QString Filter::getName() const {
        return QCoreApplication::translate("deskew::Filter", "Deskew");
    }

    PageView Filter::getView() const {
        return PAGE_VIEW;
    }

    void Filter::performRelinking(AbstractRelinker const& relinker) {
        m_ptrSettings->performRelinking(relinker);
    }

    void Filter::preUpdateUI(FilterUiInterface* const ui, PageId const& page_id) {
        m_ptrOptionsWidget->preUpdateUI(page_id);
        ui->setOptionsWidget(m_ptrOptionsWidget.get(), ui->KEEP_OWNERSHIP);
    }

    QDomElement Filter::saveSettings(ProjectWriter const& writer, QDomDocument& doc) const {
        using namespace boost::lambda;

        QDomElement filter_el(doc.createElement("deskew"));

        filter_el.setAttribute("average", m_ptrSettings->avg());
        filter_el.setAttribute("sigma", m_ptrSettings->std());
        filter_el.setAttribute("maxDeviation", m_ptrSettings->maxDeviation());

        writer.enumPages(
                [&](PageId const& page_id, int const numeric_id) {
                    this->writePageSettings(doc, filter_el, page_id, numeric_id);
                }
        );

        return filter_el;
    }

    void Filter::loadSettings(ProjectReader const& reader, QDomElement const& filters_el) {
        m_ptrSettings->clear();

        QDomElement const filter_el(filters_el.namedItem("deskew").toElement());

        m_ptrSettings->setAvg(filter_el.attribute("average").toDouble());
        m_ptrSettings->setStd(filter_el.attribute("sigma").toDouble());
        m_ptrSettings->setMaxDeviation(
                filter_el.attribute("maxDeviation", QString::number(5.0)).toDouble()
        );

        QString const page_tag_name("page");
        QDomNode node(filter_el.firstChild());
        for (; !node.isNull(); node = node.nextSibling()) {
            if (!node.isElement()) {
                continue;
            }
            if (node.nodeName() != page_tag_name) {
                continue;
            }
            QDomElement const el(node.toElement());

            bool ok = true;
            int const id = el.attribute("id").toInt(&ok);
            if (!ok) {
                continue;
            }

            PageId const page_id(reader.pageId(id));
            if (page_id.isNull()) {
                continue;
            }

            QDomElement const params_el(el.namedItem("params").toElement());
            if (params_el.isNull()) {
                continue;
            }

            Params const params(params_el);
            m_ptrSettings->setPageParams(page_id, params);
        }
    }      // Filter::loadSettings

    void Filter::writePageSettings(QDomDocument& doc, QDomElement& filter_el, PageId const& page_id,
                                   int const numeric_id) const {
        std::unique_ptr<Params> const params(m_ptrSettings->getPageParams(page_id));
        if (!params) {
            return;
        }

        QDomElement page_el(doc.createElement("page"));
        page_el.setAttribute("id", numeric_id);
        page_el.appendChild(params->toXml(doc, "params"));

        filter_el.appendChild(page_el);
    }

    intrusive_ptr<Task>
    Filter::createTask(PageId const& page_id,
                       intrusive_ptr<select_content::Task> next_task,
                       bool const batch_processing,
                       bool const debug) {
        return intrusive_ptr<Task>(
                new Task(
                        intrusive_ptr<Filter>(this), m_ptrSettings,
                        std::move(next_task), page_id, batch_processing, debug
                )
        );
    }

    intrusive_ptr<CacheDrivenTask>
    Filter::createCacheDrivenTask(intrusive_ptr<select_content::CacheDrivenTask> next_task) {
        return intrusive_ptr<CacheDrivenTask>(
                new CacheDrivenTask(m_ptrSettings, std::move(next_task))
        );
    }

    void Filter::loadDefaultSettings(PageId const& page_id) {
        if (!m_ptrSettings->isParamsNull(page_id)) {
            return;
        }
        const DefaultParams defaultParams = DefaultParamsProvider::getInstance()->getParams();
        const DefaultParams::DeskewParams& deskewParams = defaultParams.getDeskewParams();

        m_ptrSettings->setPageParams(
                page_id,
                Params(deskewParams.getDeskewAngleDeg(), Dependencies(), deskewParams.getMode())
        );
    }

    OptionsWidget* Filter::optionsWidget() {
        return m_ptrOptionsWidget.get();
    }

    void Filter::updateStatistics() {
        m_ptrSettings->updateDeviation();
    }
}  // namespace deskew