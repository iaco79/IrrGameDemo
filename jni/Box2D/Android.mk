LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := box2d_static

BOX2D1= Collision/b2BroadPhase.cpp Collision/b2CollideCircle.cpp Collision/b2CollideEdge.cpp Collision/b2CollidePolygon.cpp Collision/b2Collision.cpp
BOX2D2= Collision/b2Distance.cpp Collision/b2DynamicTree.cpp Collision/b2TimeOfImpact.cpp Collision/Shapes/b2ChainShape.cpp Collision/Shapes/b2CircleShape.cpp Collision/Shapes/b2EdgeShape.cpp
BOX2D3= Collision/Shapes/b2PolygonShape.cpp Common/b2BlockAllocator.cpp Common/b2Draw.cpp Common/b2Math.cpp Common/b2Settings.cpp Common/b2StackAllocator.cpp Common/b2Timer.cpp Dynamics/b2Body.cpp Dynamics/b2ContactManager.cpp
BOX2D4= Dynamics/b2Fixture.cpp Dynamics/b2Island.cpp Dynamics/b2World.cpp Dynamics/b2WorldCallbacks.cpp Dynamics/Contacts/b2ChainAndCircleContact.cpp Dynamics/Contacts/b2ChainAndPolygonContact.cpp Dynamics/Contacts/b2CircleContact.cpp Dynamics/Contacts/b2Contact.cpp
BOX2D5= Dynamics/Contacts/b2ContactSolver.cpp Dynamics/Contacts/b2EdgeAndCircleContact.cpp Dynamics/Contacts/b2EdgeAndPolygonContact.cpp Dynamics/Contacts/b2PolygonAndCircleContact.cpp Dynamics/Contacts/b2PolygonContact.cpp Dynamics/Joints/b2DistanceJoint.cpp
BOX2D6= Dynamics/Joints/b2FrictionJoint.cpp Dynamics/Joints/b2GearJoint.cpp Dynamics/Joints/b2Joint.cpp Dynamics/Joints/b2MouseJoint.cpp Dynamics/Joints/b2PrismaticJoint.cpp
BOX2D7= Dynamics/Joints/b2PulleyJoint.cpp Dynamics/Joints/b2RevoluteJoint.cpp Dynamics/Joints/b2RopeJoint.cpp Dynamics/Joints/b2WeldJoint.cpp Dynamics/Joints/b2WheelJoint.cpp Rope/b2Rope.cpp

BOXFULL = $(BOX2D1) $(BOX2D2) $(BOX2D3) $(BOX2D4) $(BOX2D5) $(BOX2D6) $(BOX2D7)


LOCAL_SRC_FILES := $(BOXFULL)


LOCAL_ARM_MODE   := arm 
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CFLAGS := -O3 -I./jni


include $(BUILD_STATIC_LIBRARY)

