#include <iostream>
#include "atkui/framework.h"
#include "atk/toolkit.h"

using namespace glm;
using namespace atk;

class MotionViewer : public atkui::Framework
{
public:
   MotionViewer() : atkui::Framework(atkui::Perspective)
   {
   }

   void setFileName(std::string file)
   {
      filename = file;
   }

   void setup()
   {
      BVHReader reader;
      reader.load(filename, skeleton, motion);
      motion.update(skeleton, 0);
   }

   void scene()
   {
      if (!paused)
      {
         time += dt() * timeScale;
      }
      currentFrame = motion.getKeyID(time);
      motion.update(skeleton, time);

      setColor(vec3(0, 0, 0.8));
      for (int i = 0; i < skeleton.getNumJoints(); i++)
      {
         Joint *joint = skeleton.getByID(i);
         if (joint->getParent() == 0)
            continue;

         vec3 p1 = joint->getGlobalTranslation();
         vec3 p2 = joint->getParent()->getGlobalTranslation();
         drawEllipsoid(p1, p2, 5);
      }

      drawText(paused ? "Paused" : "Playing", 10, 15);
      drawText("Current frame: " + std::to_string(currentFrame), 10, 35);
      drawText("Time scale: " + std::to_string(timeScale), 10, 55);
   }

   virtual void keyUp(int key, int mods)
   {
      if (key == 'P')
      {
         paused = !paused;
      }
      if (key == '0')
      {
         time = 0;
         currentFrame = 0;
         skeleton.setPose(motion.getKey(currentFrame));
      }
      if (paused && key == '.')
      {
         time += motion.getDeltaTime();
      }
      if (paused && key == ',')
      {
         time -= motion.getDeltaTime();
         if (time < 0)
         {
            time = motion.getDuration() - motion.getDeltaTime();
         }
      }
      if (key == ']')
      {
         timeScale += 0.1;
      }
      if (key == '[')
      {
         timeScale -= 0.1;
         if (timeScale < 0.1)
         {
            timeScale = 0.1f;
         }
      }
   }

private:
   Skeleton skeleton;
   Motion motion;

   float timeScale = 1.0f;
   int currentFrame = 0;
   bool paused = false;
   float time = 0;
   std::string filename;
};

int main(int argc, char **argv)
{

   MotionViewer viewer;
   if (argc > 1)
   {
      viewer.setFileName(argv[1]);
   }
   // default motion
   else
   {
      viewer.setFileName("../motions/Beta/jump.bvh");
   }
   viewer.run();
}
