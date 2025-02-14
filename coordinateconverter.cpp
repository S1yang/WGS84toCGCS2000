#include "coordinateConverter.h"
#include <QtMath>
#include <QFile>
#include <QTextStream>
#include <QVariantMap>
#include <QGuiApplication>
#include <QClipboard>

CoordinateConverter::CoordinateConverter(QObject *parent)
    : QObject(parent)
{
}

QVariantMap CoordinateConverter::convert(double lat, double lon, double centralMeridian)
{
    double x = 0.0, y = 0.0;
    transformWGS84ToCGCS2000GaussKruger(lat, lon, centralMeridian, x, y);

    QVariantMap result;
    result["x"] = x;
    result["y"] = y;
    return result;
}

bool CoordinateConverter::batchConvertFromFile(const QString &inputFilePath,
                                               const QString &outputFilePath,
                                               double centralMeridian)
{
    QFile inFile(inputFilePath);
    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QFile outFile(outputFilePath);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        inFile.close();
        return false;
    }

    QTextStream in(&inFile);
    QTextStream out(&outFile);

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }
        // 假设一行格式为: lat,lon
        QStringList parts = line.split(",");
        if (parts.size() < 2) {
            continue;
        }

        double lat = parts.at(0).toDouble();
        double lon = parts.at(1).toDouble();

        double x = 0.0, y = 0.0;
        transformWGS84ToCGCS2000GaussKruger(lat, lon, centralMeridian, x, y);

        // 输出格式： x,y
        out << x << "," << y << "\n";
    }

    inFile.close();
    outFile.close();
    return true;
}

void CoordinateConverter::copyToClipboard(const QString &text)
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
}

bool CoordinateConverter::saveTextToFile(const QString &text, const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream out(&file);
    out << text;
    file.close();
    return true;
}

QString CoordinateConverter::batchConvertToString(const QString &inputFilePath, double centralMeridian)
{
    QFile inFile(inputFilePath);
    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "打开输入文件失败";
    }

    QString resultString;
    QTextStream in(&inFile);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }
        // 假设每行格式为：lat,lon
        QStringList parts = line.split(",");
        if (parts.size() < 2) {
            continue;
        }

        double lat = parts.at(0).toDouble();
        double lon = parts.at(1).toDouble();
        double x = 0.0, y = 0.0;
        transformWGS84ToCGCS2000GaussKruger(lat, lon, centralMeridian, x, y);
        resultString += QString("X: %1, Y: %2\n")
                            .arg(x, 0, 'f', 10)
                            .arg(y, 0, 'f', 10);

    }
    inFile.close();
    return resultString;
}

void CoordinateConverter::transformWGS84ToCGCS2000GaussKruger(double lat, double lon, double cm,
                                                              double &x, double &y)
{
    // 1) 将度转为弧度
    double latRad = qDegreesToRadians(lat);
    double lonRad = qDegreesToRadians(lon);
    double cmRad  = qDegreesToRadians(cm);

    // 2) 椭球参数（WGS84 与 CGCS2000 非常接近）
    double a = 6378137.0;                 // 长半轴
    double f = 1.0 / 298.257222101;         // 扁率
    double e2 = 2.0*f - f*f;                // 第一偏心率平方

    // 3) 高斯-克吕格投影公式
    double N = a / qSqrt(1.0 - e2 * qSin(latRad) * qSin(latRad));
    double t = qTan(latRad) * qTan(latRad);
    double c = e2 / (1.0 - e2) * qCos(latRad) * qCos(latRad);
    double A = (lonRad - cmRad) * qCos(latRad);

    // 子午线弧长
    double M = a * ( (1 - e2/4.0 - 3.0*e2*e2/64.0 - 5.0*e2*e2*e2/256.0)*latRad
                    - (3.0*e2/8.0 + 3.0*e2*e2/32.0 + 45.0*e2*e2*e2/1024.0)*qSin(2.0*latRad)
                    + (15.0*e2*e2/256.0 + 45.0*e2*e2*e2/1024.0)*qSin(4.0*latRad)
                    - (35.0*e2*e2*e2/3072.0)*qSin(6.0*latRad) );

    double A2 = A*A;
    double A3 = A2*A;
    double A4 = A2*A2;
    double A5 = A4*A;
    double A6 = A5*A;

    y = ( M
         + N*qTan(latRad)*( A2/2.0
                               + A4/24.0*(5.0 - t + 9.0*c + 4.0*c*c)
                               + A6/720.0*(61.0 - 58.0*t + t*t + 270.0*c - 330.0*t*c)) );
    x = ( N * ( A
              + A3/6.0*(1.0 - t + c)
              + A5/120.0*(5.0 - 18.0*t + t*t + 14.0*c - 58.0*t*c)) );

    // 4) 加上假东（通常 +500000），北半球不加假北
    x += 500000.0;
}

