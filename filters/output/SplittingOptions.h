
#ifndef SCANTAILOR_SPLITTINGOPTIONS_H
#define SCANTAILOR_SPLITTINGOPTIONS_H

#include <QtXml/QDomElement>

namespace output {
    enum SplittingMode {
        BLACK_AND_WHITE_FOREGROUND,
        COLOR_FOREGROUND
    };

    class SplittingOptions {
    public:
        SplittingOptions();

        explicit SplittingOptions(QDomElement const& el);

        QDomElement toXml(QDomDocument& doc, QString const& name) const;

        bool isSplitOutput() const;

        void setSplitOutput(bool splitOutput);

        SplittingMode getSplittingMode() const;

        void setSplittingMode(SplittingMode foregroundType);

        bool operator==(const SplittingOptions& other) const;

        bool operator!=(const SplittingOptions& other) const;

        bool isOriginalBackground() const;

        void setOriginalBackground(bool originalBackground);

    private:
        static SplittingMode parseSplittingMode(const QString& str);

        static QString formatSplittingMode(SplittingMode type);


        bool splitOutput;
        SplittingMode splittingMode;
        bool originalBackground;
    };
}


#endif //SCANTAILOR_SPLITTINGOPTIONS_H
