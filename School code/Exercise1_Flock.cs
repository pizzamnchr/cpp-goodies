using System.Collections.Generic;
using AI.SteeringBehaviors.Core;

namespace AI.SteeringBehaviors.StudentAI
{
    public class Flock
    {
        // Alignment behavior strength for this Flock; updated by the UI
        public float AlignmentStrength { get; set; }

        // Cohesion behavior strength for this Flock; updated by the UI
        public float CohesionStrength { get; set; }

        // Separation behavior strength for this Flock; updated by the UI
        public float SeparationStrength { get; set; }

        // Provides a public readable (protected write) list of all agents as MovingObject instances
        public List<MovingObject> Boids { get; protected set; }

        // Average position of all boids in the flock
        public Vector3 AveragePosition { get; set; }

        // Average of the velocity (forward) vectors of all boids in the flock
        protected Vector3 AverageForward { get; set; }

        // Distance from the center of flock at which cohesion behavior maximizes its influence; updated by the UI
        public float FlockRadius { get; set; }

        // Default constructor for Flock
        public Flock()
        {
            // Initializing the default alignment behavior strength for this Flock to 1
            AlignmentStrength = 1;

            // Initializing the default cohesion behavior strength for this Flock to 1
            CohesionStrength = 1;

            // Initializing the default separation behavior strength for this Flock to 1
            SeparationStrength = 1;

            // Initializing a Vector3 for the average position of all boids in the flock
            AveragePosition = new Vector3(0, 0, 0);

            // Initializing a Vector3 for the average of the velocity (forward) vectors of all boids in the flock
            AverageForward = new Vector3(0, 0, 0);

            // Initializing the default distance from the center of flock at which cohesion behavior maximizes its influence to 50
            FlockRadius = 50;
        }

        // Method for updating the flock + its boids
        // 1. Recompute all average values
        // 2. Update each boid’s velocity based on the influence of the various component behaviors
        // 3. Call each boid’s Update() method (which will update the boid’s position)
        public virtual void Update(float deltaTime)
        {
            // Computing the average position of all boids in the flock
            AveragePosition = computeAveragePosition(Boids);

            // Computing the average of the velocity (forward) vectors of all boids in the flock
            AverageForward = computeAverageForward(Boids);

            // Iterating through all of the boids + updating their velocities based on the influence of the various component behaviors
            foreach (MovingObject theBoid in Boids)
            {
                // Computing the total acceleration using the alignment acceleration of the boid + taking into account the cohesion + separation acceleration of the boid
                Vector3 totalAcceleration = computeAlignmentAcceleration(theBoid) + computeCohesionAcceleration(theBoid) + computeSeparationAcceleration(theBoid);

                // Taking into account the cohesion acceleration of the boid
                //theAcceleration += computeCohesionAcceleration(theBoid);

                // Taking into account the separation acceleration of the boid
                //theAcceleration += computeSeparationAcceleration(theBoid);

                // Taking into account the delta time + the boid's maximum speed
                totalAcceleration *= deltaTime * theBoid.MaxSpeed;

                // The boid's velocity will be the total acceleration 
                theBoid.Velocity += totalAcceleration;

                // If the length of the boid's velocity vector is greater than the boid's maximum speed
                if (theBoid.Velocity.Length > theBoid.MaxSpeed)
                {
                    // Normalizing the boid's velocity vector
                    theBoid.Velocity.Normalize();

                    // Taking into account the boid's maximum speed for its velocity
                    theBoid.Velocity *= theBoid.MaxSpeed;
                }

                // Updating the boid every frame
                theBoid.Update(deltaTime);
            }
        }

        // Private helper method for computing the average position of all boids in the flock
        private Vector3 computeAveragePosition(List<MovingObject> givenBoids)
        {
            // Initializing the average position we will return
            Vector3 averagePosition = new Vector3(0, 0, 0);

            // Getting the total # of given boids
            int boidTotal = givenBoids.Count;

            // Iterating through all of the given boids
            foreach (MovingObject theBoid in givenBoids)
            {
                // Taking into account the boid's position for computing the average position
                averagePosition += theBoid.Position;
            }

            // Computing the average position of all boids in the flock
            averagePosition /= boidTotal;

            // Returning the computed average position
            return averagePosition;
        }

        // Private helper method for computing the average of the velocity (forward) vectors of all boids in the flock
        private Vector3 computeAverageForward(List<MovingObject> givenBoids)
        {
            // Initializing the average velocity we will return
            Vector3 averageForward = new Vector3(0, 0, 0);

            // Getting the total # of given boids
            int boidTotal = givenBoids.Count;

            // Iterating through all of the given boids
            foreach (MovingObject theBoid in givenBoids)
            {
                // Taking into account the boid's velocity for computing the average velocity
                averageForward += theBoid.Velocity;
            }

            // Computing the average of the velocity (forward) vectors of all boids in the flock
            averageForward /= boidTotal;

            // Returning the computed average velocity
            return averageForward;
        }

        // Private helper method for computing the alignment acceleration of the given boid
        private Vector3 computeAlignmentAcceleration(MovingObject givenBoid)
        {
            // Initializing the alignment acceleration using the average velocity + the given boid's maximum speed
            Vector3 alignmentAcceleration = AverageForward / givenBoid.MaxSpeed;

            // If the alignment acceleration vector's length is greater than 1
            if (alignmentAcceleration.Length > 1)
            {
                // Normalizing the alignment acceleration vector
                alignmentAcceleration.Normalize();
            }

            // Returning the computed alignment acceleration that takes into account the flock's alignment strength
            return alignmentAcceleration * AlignmentStrength;
        }

        // Private helper method for computing the cohesion acceleration of the given boid
        private Vector3 computeCohesionAcceleration(MovingObject givenBoid)
        {
            // Initializing the cohesion acceleration using the average position + the given boid's position
            Vector3 cohesionAcceleration = AveragePosition - givenBoid.Position;

            // Retrieving the cohesion acceleration vector's length
            float vectorLength = cohesionAcceleration.Length;

            // Normalizing the cohesion acceleration vector
            cohesionAcceleration.Normalize();

            // If the length of the cohesion acceleration vector is less than the flock's radius
            if (vectorLength < FlockRadius)
            {
                // Having the cohesion acceleration take into account the flock's radius
                cohesionAcceleration *= vectorLength / FlockRadius;
            }

            // Returning the computed cohesion acceleration that takes into account the flock's cohesion strength
            return cohesionAcceleration * CohesionStrength;
        }

        // Private helper method for computing the separation acceleration of the given boid
        private Vector3 computeSeparationAcceleration(MovingObject givenBoid)
        {
            // Initializing the separation acceleration we will return
            Vector3 separationAcceleration = new Vector3(0, 0, 0);

            // Iterating through all of the given boids
            foreach (MovingObject theBoid in Boids)
            {
                // If the boid is the same as the given boid
                if (theBoid == givenBoid)
                {
                    // Continuing if the boid is the same as the given boid
                    continue;
                }

                // Initializing the acceleration using the position of the given boid + the position of the current boid
                Vector3 theAcceleration = givenBoid.Position - theBoid.Position;

                // Retrieving the length of the acceleration vector
                float lengthOfAcceleration = theAcceleration.Length;

                // Computing the total safe radius
                float totalSafeRadius = theBoid.SafeRadius + givenBoid.SafeRadius;

                // If the length of the acceleration between the given + current boids is less than the total safe radius
                if (lengthOfAcceleration < totalSafeRadius)
                {
                    // Normalizing the acceleration vector
                    theAcceleration.Normalize();

                    // Taking into account the total safe radius + the acceleration vector's length for the acceleration
                    theAcceleration *= (totalSafeRadius - lengthOfAcceleration) / totalSafeRadius;

                    // Taking into account the acceleration for the separation acceleration
                    separationAcceleration += theAcceleration;
                }
            }

            // If the length of the separation acceleration vector is greater than 1
            if (separationAcceleration.Length > 1)
            {
                // Normalizing the separation acceleration vector
                separationAcceleration.Normalize();
            }

            // Returning the computed separation acceleration that takes into account the flock's separation strength
            return separationAcceleration * SeparationStrength;
        }
    }
}
