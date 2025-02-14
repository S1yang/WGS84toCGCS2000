#ifndef COORDINATECONVERTER_H
#define COORDINATECONVERTER_H

#include <QObject>
#include <QVariantMap>

class CoordinateConverter : public QObject
{
    Q_OBJECT
public:
    explicit CoordinateConverter(QObject *parent = nullptr);

    /**
     * @brief 单点转换：WGS84(经纬度) -> CGCS2000 高斯投影坐标
     * @param lat WGS84纬度 (单位：度)
     * @param lon WGS84经度 (单位：度)
     * @param centralMeridian 中央子午线 (单位：度)
     * @return QVariantMap { "x": double, "y": double }
     */
    Q_INVOKABLE QVariantMap convert(double lat, double lon, double centralMeridian = 117.0);

    /**
     * @brief 批量转换后直接返回结果字符串
     * @param inputFilePath 输入文件路径（TXT文件，每行格式：lat,lon）
     * @param centralMeridian 中央子午线
     * @return 包含所有转换结果的字符串，每行格式为 "X: ..., Y: ..."
     */
    Q_INVOKABLE QString batchConvertToString(const QString &inputFilePath, double centralMeridian = 117.0);

    // 其它接口保持不变...
    Q_INVOKABLE bool batchConvertFromFile(const QString &inputFilePath,
                                          const QString &outputFilePath,
                                          double centralMeridian = 117.0);

    Q_INVOKABLE void copyToClipboard(const QString &text);
    Q_INVOKABLE bool saveTextToFile(const QString &text, const QString &filePath);

private:
    void transformWGS84ToCGCS2000GaussKruger(double lat, double lon, double cm,
                                             double &x, double &y);
};

#endif // COORDINATECONVERTER_H
