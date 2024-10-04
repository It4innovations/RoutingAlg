#pragma once

#include <gtest/gtest.h>
#include "../Routing/Tests/IOUtils.h"
#include "../Routing/Data/Indexes/GraphMemory.h"
#include "../Routing/Data/Structures/Edge.h"
#include "../Routing/Data/Exceptions/Exceptions.h"
#include "DataPaths.h"
#include "Environment.h"

namespace {
    class GraphAndDataStructTest : public ::testing::Test {

    protected:
        virtual void SetUp() {
            this->routingGraph = GRAPH_ENV->routingGraph;
        }

        shared_ptr <GraphMemory> routingGraph;
    };

    TEST_F(GraphAndDataStructTest, NodeIdTest) {
        try {
            std::vector<int> expectedResult = {202033754, 202022293, 202145971};
            std::vector<int> nodeId = {202033754, 202022293, 202145971};

            for(unsigned int i = 0; i < expectedResult.size(); i++){
                EXPECT_EQ(expectedResult.at(i), routingGraph->GetNodeById(nodeId.at(i)).id);
            }

        }
        catch(Routing::Exception::NodeNotFoundException e){
            ADD_FAILURE() << "Node does not exist." << endl << e.what() << endl;
            return;
        }
    }

    TEST_F(GraphAndDataStructTest, NodeLatitudeLongitudeTest) {
        try {
            std::vector<float> expectedResultLat = {50.1013, 49.7979};
            std::vector<int> nodeIdLat = {202033754, 202022293};
            std::vector<float> expectedResultLong = {14.5801, 18.2559};
            std::vector<int> nodeIdLong = {202033754, 202022293};

            for(unsigned int i = 0; i < nodeIdLat.size(); i++){
                EXPECT_NEAR(expectedResultLat.at(i), routingGraph->GetNodeById(nodeIdLat.at(i)).GetLatitude(), 0.001);
                EXPECT_NEAR(expectedResultLong.at(i), routingGraph->GetNodeById(nodeIdLong.at(i)).GetLongitude(), 0.001);
            }
        }
        catch(Routing::Exception::NodeNotFoundException e){
            ADD_FAILURE() << "Node does not exist." << endl << e.what() << endl;
            return;
        }
    }

    TEST_F(GraphAndDataStructTest, NodeEdgeInCountTest) {
        try {
            std::vector<unsigned int> expectedResult = {3, 1, 1};
            std::vector<int> nodeId = {201614734, 202273813, 201884480};

            for(unsigned int i = 0; i < nodeId.size(); i++){
                EXPECT_EQ(expectedResult.at(i), routingGraph->GetEdgesIn(nodeId.at(i)).size());
            }
        }
        catch(Routing::Exception::NodeNotFoundException e){
            ADD_FAILURE() << "Node does not exist." << endl << e.what() << endl;
            return;
        }
    }

    TEST_F(GraphAndDataStructTest, NodeEdgeInTest) {
        try {
            std::vector<int> nodeId = {202033754, 202022293};
            std::vector<std::vector<int>> expectedResult = {{66840622, 67103593, 67696862}, {67135646, 68118731,
                    68578060, 68788386}};

            for (unsigned int j = 0; j < nodeId.size(); j++) {
                for (unsigned int i = 0; i < expectedResult.at(j).size(); i++) {
                    EXPECT_EQ(expectedResult.at(j).at(i), routingGraph->GetEdgesIn(nodeId.at(j)).at(i)->edgeId);
                }
            }

        }
        catch(Routing::Exception::NodeNotFoundException e){
            ADD_FAILURE() << "Node does not exist." << endl << e.what() << endl;
            return;
        }
        catch(out_of_range e){
            ADD_FAILURE() << "Expected more IN edges." << endl << e.what() << endl;
            return;
        }
    }

    TEST_F(GraphAndDataStructTest, NodeEdgeOutCountTest) {
        try {
            std::vector<unsigned int> expectedResult = {3, 1, 1};
            std::vector<int> nodeId = {201614734, 202273813, 201884480};

            for(unsigned int i = 0; i < nodeId.size(); i++){
                EXPECT_EQ(expectedResult.at(i), routingGraph->GetEdgesOut(nodeId.at(i)).size());
            }

        }
        catch(Routing::Exception::NodeNotFoundException e){
            ADD_FAILURE() << "Node does not exist." << endl << e.what() << endl;
            return;
        }
    }

    TEST_F(GraphAndDataStructTest, NodeEdgeOutTest) {
        try {
            std::vector<int> nodeId = {202033754, 202022293};
            std::vector<std::vector<int>> expectedResult = {{68095353, 68095352, 68095354}, {68072896, 68072898,
                    68072897, 68072899}};

            for (unsigned int j = 0; j < nodeId.size(); j++) {
                for (unsigned int i = 0; i < expectedResult.at(j).size(); i++) {
                    EXPECT_EQ(expectedResult.at(j).at(i), routingGraph->GetEdgesOut(nodeId.at(j)).at(i)->edgeId);
                }
            }
        }
        catch(Routing::Exception::NodeNotFoundException e){
            ADD_FAILURE() << "Node does not exist." << endl << e.what() << endl;
            return;
        }
        catch(out_of_range e){
            ADD_FAILURE() << "Expected more OUT edges." << endl << e.what() << endl;
            return;
        }
    }

    TEST_F(GraphAndDataStructTest, EdgeTotalCountTest) {
        EXPECT_EQ(2223222, routingGraph->GetEdgeCount());
    }

    TEST_F(GraphAndDataStructTest, EdgeDataTest) {
        try {
            //[expectedEdgeId, speed, funcClass, lanes, incline, maxWeight, maxHeight, maxAxleLoad, maxWidth, maxLength]
            std::vector<int> idNode = {201614734, 202273813, 201884480};
            std::vector<std::vector<int>> expectedData = {{67926422, 67089767, 68097368}, {10, 10, 5}, {6, 6, 7}, {1, 1, 1} , {0, 0, 0}, {65535, 65535, 65535},
                                                          {65535, 65535, 65535}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}};

            for(unsigned int i = 0; i<idNode.size();i++){
                EXPECT_EQ(routingGraph->GetNodeById(idNode[i]).edgesIn.at(0)->edgeId, expectedData[0][i]);
                EXPECT_EQ(routingGraph->GetNodeById(idNode[i]).edgesIn.at(0)->edgeData->speed, expectedData[1][i]);
                EXPECT_EQ(routingGraph->GetNodeById(idNode[i]).edgesIn.at(0)->edgeData->funcClass, expectedData[2][i]);
                EXPECT_EQ(routingGraph->GetNodeById(idNode[i]).edgesIn.at(0)->edgeData->lanes, expectedData[3][i]);
                EXPECT_EQ(routingGraph->GetNodeById(idNode[i]).edgesIn.at(0)->edgeData->incline, expectedData[4][i]);
                EXPECT_EQ(routingGraph->GetNodeById(idNode[i]).edgesIn.at(0)->edgeData->maxWeight, expectedData[5][i]);
                EXPECT_EQ(routingGraph->GetNodeById(idNode[i]).edgesIn.at(0)->edgeData->maxHeight, expectedData[6][i]);
                EXPECT_EQ(routingGraph->GetNodeById(idNode[i]).edgesIn.at(0)->edgeData->maxAxleLoad, expectedData[7][i]);
                EXPECT_EQ(routingGraph->GetNodeById(idNode[i]).edgesIn.at(0)->edgeData->maxWidth, expectedData[8][i]);
                EXPECT_EQ(routingGraph->GetNodeById(idNode[i]).edgesIn.at(0)->edgeData->maxLength, expectedData[9][i]);
            }
        }
        catch(Routing::Exception::NodeNotFoundException e){
            ADD_FAILURE() << "Node does not exist." << endl << e.what() << endl;
            return;
        }
    }

    TEST_F(GraphAndDataStructTest, EdgeToSegmentTest)
    {
        const int edgeId = 66677163;

        Routing::Algorithms::Segment segment = routingGraph->EdgeToSegment(edgeId);

        EXPECT_EQ(segment.edgeId, edgeId);
        EXPECT_EQ(segment.length, 23);
        EXPECT_EQ(segment.speed, 30);
        EXPECT_FLOAT_EQ(segment.time, (float) segment.length / segment.speed);
    }
}
