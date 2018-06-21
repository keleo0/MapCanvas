#include "MapManager.h"

extern QObject *g_pQmlRoot;

static const QString regionLayerName = "CanvasRegions";     //预置区域块层对象名称
static const QString polyLineLayerName = "CanvasPolyLines"; //预置折线层对象名称
static const QString pointsLayerName = "CanvasPoints";      //预置点层对象名称
static const QString textLayerName = "CanvasTexts";         //预置文本层对象名称

MapPoint::MapPoint()
{
    borderColor = QColor(Qt::black);
    innerColor = QColor(Qt::yellow);
}

MapPolyLine::~MapPolyLine()
{
    if(polyPoints.count() > 0)
        polyPoints.clear();
}

MapRegion::~MapRegion()
{
    if(mapFields.count() > 0)
        mapFields.clear();
    if(regionPoints.count() > 0)
        regionPoints.clear();
}

QPointF MapManager::convertToRelativeCoord(double x,double y)
{
    QPointF point;
    viewWidthByPixel = getDefaultSceneSize().width();
    viewHeightByPixel = getDefaultSceneSize().height();
    point = QPointF((x - sceneLeftTopLong) / viewWidthByLong,
                    1 - (y - sceneLeftTopLat) / viewHeightByLat);
    return point;
}

bool MapManager::GetLayerItems(QString layname)
{
    mitab_handle handle;
    mitab_feature feature;
    int feature_id, num_fields;
    int featurecount;
    int featuretype;
    QStringList fieldnames;

    handle = mitab_c_open(layname.toLatin1().data());
    if(handle == 0)
    {
        qDebug()<<"open layer failed ~~";
        return false;
    }

    featurecount = mitab_c_get_feature_count(handle);

    num_fields = mitab_c_get_field_count(handle);
    for(int i = 0;i < num_fields; i++)
    {
        fieldnames.append(mitab_c_get_field_name(handle, i));
    }

    for(feature_id = mitab_c_next_feature_id(handle, -1);
        feature_id != -1;
        feature_id = mitab_c_next_feature_id(handle, feature_id) )
    {
        //qDebug()<<"feature id="<<feature_id;
        feature = mitab_c_read_feature(handle, feature_id);
        featuretype = mitab_c_get_type(feature);
        //qDebug()<<"feature type is"<<featuretype;
        if((featuretype >= TABFC_Point) && (featuretype <= TABFC_Ellipse)) //如果是可显示对象
        {
            switch(featuretype)
            {
            case TABFC_Point:
                //qDebug() << "GetPoint";
                GetPoint(feature, fieldnames);
                break;
            case TABFC_FontPoint:
                //qDebug()<<"TABFC_FontPoint";
                break;
            case TABFC_CustomPoint:
                //qDebug()<<"TABFC_CustomPoint";
                break;
            case TABFC_Text:
                //qDebug()<<"TABFC_Text";
                break;
            case TABFC_Polyline:
                //qDebug() << "GetPolyLine";
                GetPolyLine(feature,fieldnames);
                break;
            case TABFC_Arc:
                //qDebug()<<"TABFC_Arc";
                break;
            case TABFC_Region:
                //qDebug() << "GetRegion";
                GetRegion(feature,fieldnames);
                break;
            case TABFC_Rectangle:
                //qDebug()<<"TABFC_Rectangle";
                break;
            case TABFC_Ellipse:
                //qDebug()<<"TABFC_Ellipse";
                break;
            default:
                break;
            }
        }//end if
        mitab_c_destroy_feature(feature);
    }//end for(n=0;n<featurecount;n++)
    mitab_c_close(handle);
    return true;
}

void MapManager::GetPolyLine(mitab_feature feature,QStringList &fields)
{
    MapPolyLine *mapPolyline;
    int parts;
    int vertexs;
    QList<double> longs; //经度集
    QList<double> lats;  //纬度集

    parts = mitab_c_get_parts(feature); //取对象环回折线段数量
    longs.clear();
    lats.clear();
    for(int i = 0; i < parts; i++)
    {
        vertexs = mitab_c_get_vertex_count(feature, i); //返回对象顶点数
        //qDebug()<<"vertexs="<<vertexs;
        for(int j = 0; j < vertexs; j++)
        {
            longs.append(mitab_c_get_vertex_x(feature, i, j));
            lats.append(mitab_c_get_vertex_y(feature, i, j));
        }
    }
    if((longs.count() > 0) && (longs.count() == lats.count()))
    {
        mapPolyline = new MapPolyLine();    //新建折线对象
        mapPolyline->lineWidth = mitab_c_get_pen_width(feature);
        mapPolyline->lineColor = mitab_c_get_pen_color(feature);
        for(int i = 0; i < longs.count(); i++)
        {
            mapPolyline->polyPoints.append(new QPointF(convertToRelativeCoord(longs.at(i),lats.at(i))));
        }
        m_mapPolyLines.append(mapPolyline); //保存折线对象
    }
}

void MapManager::GetRegion(mitab_feature feature,QStringList &fields)
{
    QPolygonF polygon;  //计算区域中心坐标所用对象，中心坐标作为文本位置

    MapField *field;
    int parts;
    int vertexs;
    QList<double> longs; //经度集
    QList<double> lats;  //纬度集

    //pencolor = mitab_c_get_pen_color(feature);
    //penpattern = mitab_c_get_pen_pattern(feature);
    //penwidth = mitab_c_get_pen_width(feature);
    //pen.setColor(pencolor);
    //pen.setStyle(Qt::SolidLine);
    //pen.setWidth(penwidth);
    //brushcolor = mitab_c_get_brush_fgcolor(feature);
    //brushpattern = mitab_c_get_brush_pattern(feature);
    //brush.setColor(QColor(brushcolor));

    MapRegion *mapRegion = new MapRegion();
    MapText *mapText = new MapText();

    for(int i = 0; i < fields.count(); i++)
    {
        field = new MapField;
        field->fieldName = fields.at(i);
        field->fieldContent = QString::fromLocal8Bit(mitab_c_get_field_as_string(feature,i));
        mapRegion->mapFields.append(field);
        if(field->fieldName == "Name")
        {
            mapText->mapText = field->fieldContent; //文本对象文本赋值
        }
    }
    parts = mitab_c_get_parts(feature); //取对象环回折线段数量
    longs.clear();
    lats.clear();
    for(int i = 0; i < parts; i++)
    {
        vertexs = mitab_c_get_vertex_count(feature,i); //返回对象顶点数
        for(int j = 0; j < vertexs; j++)
        {
            longs.append(mitab_c_get_vertex_x(feature,i,j));
            lats.append(mitab_c_get_vertex_y(feature,i,j));
        }
    }
    if((longs.count() > 0) && (longs.count() == lats.count()))
    {
        mapRegion->borderWidth = mitab_c_get_pen_width(feature);
        mapRegion->borderColor = mitab_c_get_pen_color(feature);
        mapRegion->innerColor = mitab_c_get_brush_fgcolor(feature);
        for(int i = 0; i < longs.count(); i++)
        {
            QPointF *point = new QPointF(convertToRelativeCoord(longs.at(i),lats.at(i)));
            mapRegion->regionPoints.append(point);
            polygon.append(*point);
        }
        mapText->textCoord = polygon.boundingRect().center();   //文本对象坐标赋值
        m_mapTexts.append(mapText);
        m_mapRegions.append(mapRegion);
    }
}

void MapManager::GetPoint(mitab_feature feature,QStringList &fields)
{
    MapPoint *mapPoint;
    int parts;
    int vertexs;
    QList<double> longs; //经度集
    QList<double> lats;  //纬度集

    //pencolor = mitab_c_get_pen_color(feature);
    //penpattern = mitab_c_get_pen_pattern(feature);
    //penwidth = mitab_c_get_pen_width(feature);
    //pen.setColor(pencolor);
    //pen.setStyle(Qt::SolidLine);
    //pen.setWidth(penwidth);
    //brushcolor = mitab_c_get_brush_fgcolor(feature);
    //brushpattern = mitab_c_get_brush_pattern(feature);
    //brush.setColor(QColor(Qt::lightGray));

    parts = mitab_c_get_parts(feature);
    longs.clear();
    lats.clear();
    for(int i = 0; i < parts; i++)
    {
        vertexs = mitab_c_get_vertex_count(feature,i); //返回对象顶点数
        //qDebug()<<"vertexs="<<vertexs;
        for(int j = 0; j < vertexs; j++)
        {
            longs.append(mitab_c_get_vertex_x(feature,i,j));
            lats.append(mitab_c_get_vertex_y(feature,i,j));
        }
    }
    if(longs.count()>0 && (longs.count()==lats.count()))
    {
        for(int i = 0; i < longs.count(); i++)
        {
            mapPoint = new MapPoint();
            mapPoint->borderColor = mitab_c_get_pen_color(feature);
            mapPoint->innerColor = mitab_c_get_brush_fgcolor(feature);
            mapPoint->relativePoint = convertToRelativeCoord(longs.at(i),lats.at(i));
            m_mapPoints.append(mapPoint);
        }
    }
}

void MapManager::createLayers()
{
    CreateRegionsLayer();
    CreatePolyLinesLayer();
    CreatePointsLayer();
    //CreateTextsLayer();
}

void MapManager::CreatePolyLinesLayer()
{
    if(m_mapPolyLines.count() > 0)
    {
        if(this->findChildren<QQuickItem *>(polyLineLayerName, Qt::FindDirectChildrenOnly).count() == 0)
        {
            if(QMetaObject::invokeMethod(this, "createCanvasLayer", Qt::DirectConnection,
                                         Q_ARG(QVariant, polyLineLayerName)))   //调用qml函数创建图层
                m_polyLineLayerObject = this->findChild<QObject *>(polyLineLayerName, Qt::FindDirectChildrenOnly);
            else
                qDebug() << polyLineLayerName << "qml layer create failed ~~~~";
        }
        if(m_polyLineLayerObject)   //如果创建成功则对对象内数据结构进行赋值
        {
            m_polyLineLayerObject->setProperty("linesCount", QVariant(m_mapPolyLines.count()));
            QList<QVariant> linesCountList;
            QList<QVariant> linesColorList;
            QList<QVariant> linesWidthList;
            QList<QVariant> linesCoordXList;
            QList<QVariant> linesCoordYList;
            foreach(MapPolyLine *polyLine, m_mapPolyLines)
            {
                linesCountList.append(polyLine->polyPoints.count());
                linesColorList.append(polyLine->lineColor.name(QColor::HexRgb));
                linesWidthList.append(polyLine->lineWidth);
                QList<QVariant> pXList;
                QList<QVariant> pYList;
                foreach(QPointF *point, polyLine->polyPoints)
                {
                    pXList.append(QVariant(point->x()));
                    pYList.append(QVariant(point->y()));
                }
                linesCoordXList.append(QVariant(pXList));
                linesCoordYList.append(QVariant(pYList));
            }
            m_polyLineLayerObject->setProperty("lineWidth", QVariant(linesWidthList));
            m_polyLineLayerObject->setProperty("lineColor", QVariant(linesColorList));
            m_polyLineLayerObject->setProperty("pointsCountPerLine", QVariant(linesCountList));
            m_polyLineLayerObject->setProperty("pointsCoordXPerLine", QVariant(linesCoordXList));
            m_polyLineLayerObject->setProperty("pointsCoordYPerLine", QVariant(linesCoordYList));
        }
    }
}

void MapManager::CreateRegionsLayer()
{
    if(m_mapRegions.count() > 0)
    {
        if(this->findChildren<QQuickItem *>(regionLayerName, Qt::FindDirectChildrenOnly).count() == 0)
        {
            if(QMetaObject::invokeMethod(this, "createCanvasLayer", Qt::DirectConnection,
                                         Q_ARG(QVariant, regionLayerName))) //调用qml函数创建图层
                m_regionLayerObject = this->findChild<QObject *>(regionLayerName, Qt::FindDirectChildrenOnly);
            else
                qDebug() << regionLayerName << "qml layer create failed ~~~~";
        }
        if(m_regionLayerObject) //如果创建成功则对对象内数据结构进行赋值
        {
            m_regionLayerObject->setProperty("regionsCount", QVariant(m_mapRegions.count()));
            QList<QVariant> regionsCountList;
            QList<QVariant> regionsBorderWidthList;
            QList<QVariant> regionsBorderColorList;
            QList<QVariant> regionsInnerColorList;
            QList<QVariant> regionsCoordXList;
            QList<QVariant> regionsCoordYList;
            foreach(MapRegion *region, m_mapRegions)
            {
                regionsCountList.append(region->regionPoints.count());
                regionsBorderWidthList.append(region->borderWidth);
                regionsBorderColorList.append(region->borderColor.name(QColor::HexRgb));
                regionsInnerColorList.append(region->innerColor.name(QColor::HexRgb));
                QList<QVariant> pXList;
                QList<QVariant> pYList;
                foreach(QPointF *point, region->regionPoints)
                {
                    pXList.append(QVariant(point->x()));
                    pYList.append(QVariant(point->y()));
                }
                regionsCoordXList.append(QVariant(pXList));
                regionsCoordYList.append(QVariant(pYList));
            }
            m_regionLayerObject->setProperty("regionsBorderWidth", QVariant(regionsBorderWidthList));
            m_regionLayerObject->setProperty("regionsBorderColor", QVariant(regionsBorderColorList));
            m_regionLayerObject->setProperty("regionsInnerColor", QVariant(regionsInnerColorList));
            m_regionLayerObject->setProperty("pointsCountPerRegion", QVariant(regionsCountList));
            m_regionLayerObject->setProperty("pointsCoordXPerRegion", QVariant(regionsCoordXList));
            m_regionLayerObject->setProperty("pointsCoordYPerRegion", QVariant(regionsCoordYList));
        }
    }
}

void MapManager::CreatePointsLayer()
{
    if(m_mapPoints.count() > 0)
    {
        if(this->findChildren<QQuickItem *>(pointsLayerName, Qt::FindDirectChildrenOnly).count() == 0)
        {
            if(QMetaObject::invokeMethod(this, "createCanvasLayer", Qt::DirectConnection,
                                         Q_ARG(QVariant, pointsLayerName))) //调用qml函数创建图层
                m_pointsLayerObject = this->findChild<QObject *>(pointsLayerName, Qt::FindDirectChildrenOnly);
            else
                qDebug() << pointsLayerName << "qml layer create failed ~~~~";
        }
        if(m_pointsLayerObject) //如果创建成功则对对象内数据结构进行赋值
        {
            m_pointsLayerObject->setProperty("pointSide", QVariant(3));
            m_pointsLayerObject->setProperty("pointsCount", QVariant(m_mapPoints.count()));
            m_pointsLayerObject->setProperty("penColor", QVariant(m_mapPoints[0]->borderColor.name(QColor::HexRgb)));
            m_pointsLayerObject->setProperty("paletteColor", QVariant(m_mapPoints[0]->innerColor.name(QColor::HexRgb)));
            QList<QVariant> pList;
            QList<MapPoint *>::iterator i;
            for(i = m_mapPoints.begin(); i != m_mapPoints.end(); ++i)
            {
                pList.append(QVariant((*i)->relativePoint));
            }
            m_pointsLayerObject->setProperty("pointsList", QVariant(pList));
        }
    }
}

void MapManager::CreateTextsLayer()
{
    if(m_mapTexts.count() > 0)
    {
        if(this->findChildren<QQuickItem *>(textLayerName, Qt::FindDirectChildrenOnly).count() == 0)
        {
            if(QMetaObject::invokeMethod(this, "createCanvasLayer", Qt::DirectConnection,
                                         Q_ARG(QVariant, textLayerName)))   //调用qml函数创建图层
                m_textsLayerObject = this->findChild<QObject *>(textLayerName, Qt::FindDirectChildrenOnly);
            else
                qDebug() << textLayerName << "qml layer create failed ~~~~";
        }
        if(m_textsLayerObject)  //如果创建成功则对对象内数据结构进行赋值
        {
            QStringList textsList;
            QList<QVariant> textsCoordList;

            m_textsLayerObject->setProperty("textsCount", QVariant(m_mapTexts.count()));
            foreach (MapText *text, m_mapTexts)
            {
                textsList.append(text->mapText);
                textsCoordList.append(text->textCoord);
            }
            m_textsLayerObject->setProperty("textsList", QVariant(textsList));
            m_textsLayerObject->setProperty("textsCoordList", QVariant(textsCoordList));
        }
    }
}

MapManager::MapManager(QQuickItem *parent) : QQuickItem(parent)
{
    sceneLeftTopLong = 118.61;  //根据地图文件外部信息赋初值
    sceneLeftTopLat = 31.9174;
    sceneRightBottomLong = 118.98;
    sceneRightBottomLat = 32.178;
    viewWidthByLong = sceneRightBottomLong - sceneLeftTopLong;
    viewHeightByLat = sceneRightBottomLat - sceneLeftTopLat;

    m_regionLayerObject = NULL;
    m_polyLineLayerObject = NULL;
    m_pointsLayerObject = NULL;
    m_textsLayerObject = NULL;

    getMapInfo();
}

MapManager::~MapManager()
{
    if(m_mapRegions.count() > 0)
        m_mapRegions.clear();
    if(m_mapPolyLines.count() > 0)
        m_mapPolyLines.clear();
    if(m_mapPoints.count() > 0)
        m_mapPoints.clear();
    if(m_mapTexts.count() > 0)
        m_mapTexts.clear();
}

void MapManager::getMapInfo()
{
#if 0
    QFileInfoList InfoList = QDir(qApp->applicationDirPath().append("/maps").entryInfoList());
    foreach(QFileInfo fileInfo, InfoList)
    {
        if(!fileInfo.isFile())
            continue;
        if(0 == fileInfo.suffix().compare(("mif"), Qt::CaseInsensitive))
        {
            if(layer->GetLayerItems(fileInfo.filePath()))
                m_mapLayers.append(layer);
        }
    }
#else
    //读入地图文件
    GetLayerItems("./maps/Adm_Area.mif");   //region
    GetLayerItems("./maps/Landuse.mif");    //region
    GetLayerItems("./maps/Block.mif");      //region
    GetLayerItems("./maps/Road.mif");       //lines
    GetLayerItems("./maps/RailWay.mif");    //lines
    //GetLayerItems("./maps/LandMark.mif");   //points
    //GetLayerItems("./maps/Node.mif");     //points
    //GetLayerItems("./maps/Adm_LandMark.mif");//points
    //GetLayerItems("./maps/POI.mif");      //points
#endif
}

void MapManager::doMapCanvasWork()
{
    if(this->setProperty("bIsStart", QVariant(true)))
    {
        //请求各图层进行绘制操作
        QMetaObject::invokeMethod(m_regionLayerObject, "requestPaint", Qt::DirectConnection);
        QMetaObject::invokeMethod(m_polyLineLayerObject, "requestPaint", Qt::DirectConnection);
        QMetaObject::invokeMethod(m_pointsLayerObject, "requestPaint", Qt::DirectConnection);
        QMetaObject::invokeMethod(m_textsLayerObject, "requestPaint", Qt::DirectConnection);
    }
}

QSize MapManager::getDefaultSceneSize()
{
    return QSize(2000, 2000);
}
