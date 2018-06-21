#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <QObject>
#include <QColor>
#include <QPointF>
#include <QHash>
#include <QQuickItem>
#include <QList>
#include <QFileInfo>
#include <QPolygonF>

#include "mitab.h"
#include "mitab_capi.h"

struct MapField
{
    QString fieldName;  //region属性名称
    QString fieldContent;   //region属性内容
};

class MapPoint
{
public:
    MapPoint();
public:
    QColor borderColor; //本点边界颜色
    QColor innerColor;  //本点填充颜色
    QPointF relativePoint;  //本点坐标
};

class MapPolyLine
{
public:
    MapPolyLine(){}
    ~MapPolyLine();
public:
    int lineWidth;  //本折线绘制宽度
    QColor lineColor;   //本折线绘制颜色
    QList<QPointF *> polyPoints;    //本折线绘制所有点坐标
};

class MapRegion
{
public:
    MapRegion(){}
    ~MapRegion();
public:
    QList<MapField *> mapFields;    //本区域信息
    int borderWidth;    //本区域绘制边界宽度
    QColor borderColor; //本区域绘制边界颜色
    QColor innerColor;  //本区域绘制填充颜色
    QList<QPointF *> regionPoints;  //本区域绘制边界所有点坐标
};

class MapText
{
public:
    MapText(){}
    ~MapText(){}
public:
    QString mapText;    //文本内容
    QPointF textCoord;  //文本相对坐标
};

class MapManager : public QQuickItem
{
    Q_OBJECT
public:
    explicit MapManager(QQuickItem *parent = 0);
    ~MapManager();

public:
    double sceneLeftTopLong;        //画布左上角代表的经度
    double sceneLeftTopLat;         //画布左上角代表的纬度
    double sceneRightBottomLong;    //画布右下角代表的经度
    double sceneRightBottomLat;     //画布右下角代表的纬度
    double viewWidthByPixel;        //当前显示区域的宽度
    double viewHeightByPixel;       //当前显示区域的高度
    double viewWidthByLong;         //当前显示区域经度的范围
    double viewHeightByLat;         //当前显示区域纬度的范围

    enum DrawSectionType {  //绘制阶段指示
        Points = 1,
        PolyLines,
        Regions,
        Text
    };

    QPointF convertToRelativeCoord(double x,double y); //转换为相对位置坐标

public:
    QObject *m_regionLayerObject;   //区域层对象指针
    QObject *m_polyLineLayerObject; //折线层对象指针
    QObject *m_pointsLayerObject;   //点层对象指针
    QObject *m_textsLayerObject;    //文本层对象指针

    QList<MapRegion *> m_mapRegions;    //所有多边形数据
    QList<MapPolyLine *> m_mapPolyLines;    //所有折线数据
    QList<MapPoint *> m_mapPoints;  //所有点数据
    QList<MapText *> m_mapTexts;    //所有文本数据

public:
    bool GetLayerItems(QString layname);    //获得文件数据
    void GetRegion(mitab_feature feature,QStringList &fields);  //获得每文件中多边形数据
    void GetPolyLine(mitab_feature feature,QStringList &fields);    //获得每文件中折线数据
    void GetPoint(mitab_feature feature,QStringList &fields);   //获得每文件中点数据
    void GetText(mitab_feature feature,QStringList &fields);  //获得每文件中文本数据

signals:

public slots:
    void getMapInfo(); //读地图文件
    void doMapCanvasWork(); //开始绘制
    QSize getDefaultSceneSize(); //预置scene尺寸
    void createLayers();   //创建图层
    void CreateRegionsLayer();  //创建区域块图层
    void CreatePolyLinesLayer();    //创建折线图层
    void CreatePointsLayer();   //创建点图层
    void CreateTextsLayer();   //创建文本图层
};

#endif // MAPMANAGER_H
