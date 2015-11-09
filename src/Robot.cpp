#include "Robot.hpp"
#include <iostream>
#include <smurf/Smurf.hpp>
#include <envire_core/items/Transform.hpp>
#include <base/Time.hpp>
#include <envire_core/items/Item.hpp>
#include <configmaps/ConfigData.h>



void envire::envire_smurf::Robot::welcome()
{
    std::cout << "You successfully compiled and executed the envire_smurf Project. Welcome!" << std::endl;
}

void envire::envire_smurf::Robot::loadFromSmurf(envire::core::TransformGraph &graph, const std::string& path)
{
    smurf::Robot robot;
    robot.loadFromSmurf(path);
    // Frames
    std::vector<smurf::Frame *> frames= robot.getFrames();
    for(std::vector<smurf::Frame *>::iterator it = frames.begin(); it != frames.end(); ++it)
    {
        std::string frame_id = (*it)->getName();
        graph.addFrame(frame_id);
        boost::shared_ptr<envire::core::Item< smurf::Frame * > >link_itemPtr (new  envire::core::Item<smurf::Frame *> );
        link_itemPtr->setData(*it);
        graph.addItemToFrame(frame_id, link_itemPtr);
    }
//////////////////////////////////////////////////////////////adding sensors///////////////////////////////////////////////////////////////////////////
    std::vector<smurf::Sensor *> robot_Sensors= robot.getSensors();
    std::string frame_id;
    for(std::vector<smurf::Sensor *>::iterator it = robot_Sensors.begin(); it != robot_Sensors.end(); ++it)
    {
        frame_id=(*it)->getattachmentPoint()->getName();
//        std::cout<<"------------------------------------------" <<std::endl;
//        std::cout<<"frame_id: "<<frame_id <<std::endl;
        boost::shared_ptr<envire::core::Item< smurf::Sensor*  > > sensor_itemPtr (new  envire::core::Item< smurf::Sensor* > );
        sensor_itemPtr-> setData(*it);
        graph.addItemToFrame(frame_id, sensor_itemPtr);
    }
    // Static Transformations: All transformations are considered static initially
    std::vector<smurf::StaticTransformation *> staticTfs= robot.getStaticTransforms();
    for(std::vector<smurf::StaticTransformation *>::iterator it = staticTfs.begin(); it != staticTfs.end(); ++it) {
        smurf::Frame source = (*it) -> getSourceFrame();
        envire::core::FrameId sourceId = source.getName();
        smurf::Frame target = (*it) -> getTargetFrame();
        envire::core::FrameId targetId = target.getName();
        Eigen::Affine3d tf_smurf = (*it) -> getTransformation();
        base::Time time = base::Time::now();
        base::TransformWithCovariance tf_cov(tf_smurf);
        envire::core::Transform envire_tf(time, tf_cov);
        graph.addTransform(sourceId, targetId, envire_tf);
        boost::shared_ptr<envire::core::Item<smurf::StaticTransformation*  > > joint_itemPtr (new  envire::core::Item< smurf::StaticTransformation* > );
        joint_itemPtr->setData(*it);
        graph.addItemToFrame(sourceId,joint_itemPtr);
    }
}

bool envire::envire_smurf::Robot::frameHas(envire::core::TransformGraph &graph,FRAME_ITEM_TYPE itemType, envire::core::FrameId frameID)
{
    //envire::core::Frame frame=graph.getFrame(frameID);
    using namespace boost;
    bool has_item=false;
    switch (itemType)
    {
        case SENSOR :
        {
//            std::cout << "item is sensor";
            envire::core::TransformGraph::ItemIterator<envire::core::Item<smurf::Sensor*>::Ptr> begin, end;
            tie(begin, end) = graph.getItems<envire::core::Item<smurf::Sensor*>::Ptr>(frameID);
            if(begin!=end)
                has_item=true;
            break;

        }

        case JOINT:
        {
//            std::cout << "item is joint";
            envire::core::TransformGraph::ItemIterator<envire::core::Item<smurf::StaticTransformation*>::Ptr> begin, end;
            tie(begin, end) = graph.getItems<envire::core::Item<smurf::StaticTransformation*>::Ptr>(frameID);
            if(begin!=end)
                has_item=true;
            break;
        }

        case LINK:
        {
//            std::cout << "item is link";
            envire::core::TransformGraph::ItemIterator<envire::core::Item<smurf::Frame *>::Ptr> begin, end;
            tie(begin, end) = graph.getItems<envire::core::Item<smurf::Frame *>::Ptr>(frameID);
            if(begin!=end)
                has_item=true;
            break;
        }


    }

    return has_item;
}

std::vector<envire::core::FrameId>  envire::envire_smurf::Robot::getTransformFrames(envire::core::FrameId &sourceFrame,envire::core::FrameId &targetFrame, envire::core::TransformGraph &graph)
{
    return graph.getPath(sourceFrame, targetFrame);
}
