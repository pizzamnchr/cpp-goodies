using Robocode;
using Robocode.Util;

using System;
using System.Collections.Generic;
using System.Drawing;

namespace CAP4053.Student
{
    public class PizzaBot : TeamRobot
    {
        private PizzaBotFSM myState;

        public override void Run()
        {
            // Adding the team color to the PizzaBot
            SetColors(Color.Red, Color.LightSkyBlue, Color.DodgerBlue, Color.LightGoldenrodYellow, Color.Green);

            // Having the radar turn independently from the gun's turn
            IsAdjustRadarForGunTurn = true;

            // Having the radar turn independently from the robot's turn
            IsAdjustRadarForRobotTurn = true;

            // Having the gun turn independently from the robot's turn
            IsAdjustGunForRobotTurn = true;

            // Instantiating the state
            myState = new PizzaBotFSM(this);

            while (true)
            {
                // Updating the FSM
                myState.Update();

                // Executing pending actions / continuing in process actions
                Execute();
            }
        }

        public override void OnMessageReceived(MessageEvent myEvent)
        {
            // Updating the state if we received a message from a teammate
            myState.UpdateCurrentEvent(myEvent);
        }

        public override void OnRobotDeath(RobotDeathEvent myEvent)
        {
            // Updating the state if we received a message from a teammate
            myState.UpdateCurrentEvent(myEvent);
        }

        public override void OnScannedRobot(ScannedRobotEvent myEvent)
        {
            // Updating the state if we scanned a robot
            myState.UpdateCurrentEvent(myEvent);
        }

        public override void OnHitWall(HitWallEvent myEvent)
        {
            // Updating the state if we hit a wall
            myState.UpdateCurrentEvent(myEvent);
        }

        public override void OnHitByBullet(HitByBulletEvent myEvent)
        {
            // Updating the state if we get hit by a bullet
            myState.UpdateCurrentEvent(myEvent);
        }
    }

    public class PizzaBotFSM
    {
        // Declaring our pizzaBot
        private PizzaBot pizzaBot;

        // Declaring a bool for when the pizzaBot locates an enemy
        private bool enemyLocated;

        // Declaring the direction of the pizzaBot
        private double robotDirection;

        // Declaring the current event of the pizzaBot
        private Action<Event> currentEvent;

        // Declaring the pizzaBot's current target
        private string currentTarget;

        // Declaring the dictionary of enemy robots
        private Dictionary<string, ScannedRobotEvent> enemyRobots;

        // Declaring the number of teammates alive
        private int teammateCount;

        // Declaring the number of enemies alive
        private int enemyCount;

        // PizzaBotFSM's constructor
        public PizzaBotFSM(PizzaBot myRobot)
        {
            // Initializing pizzaBot
            pizzaBot = myRobot;

            // Initializing the pizzaBot's current event
            currentEvent = ScannedRobot;

            // Initializing pizzaBot's current target
            currentTarget = "";

            // Initializing the pizzaBot's direction
            robotDirection = 1;

            // Initializing the number of teammates alive
            teammateCount = 4;

            // Initializing the number of enemies alive
            enemyCount = 4;

            // Initializing pizzaBot's dict of enemy robots
            enemyRobots = new Dictionary<string, ScannedRobotEvent>();
        }

        // Function for setting the current event
        public void UpdateCurrentEvent(Event inputEvent)
        {
            // If pizzaBot received a message from a teammate
            if (inputEvent is MessageEvent)
            {
                currentEvent = MessageReceived;
            }
            // If a robot died on the battlefield
            else if (inputEvent is RobotDeathEvent)
            {
                currentEvent = RobotDied;
            }
            // If pizzaBot scanned a robot
            else if (inputEvent is ScannedRobotEvent)
            {
                currentEvent = ScannedRobot;
            }
            // Else if pizzaBot hits a wall
            else if (inputEvent is HitWallEvent)
            {
                currentEvent = OnHitWall;
            }
            // Else if pizzaBot is hit by a bullet
            else if (inputEvent is HitByBulletEvent)
            {
                currentEvent = OnHitByBullet;
            }

            currentEvent(inputEvent);
        }

        // The 3 states that can easily be extended
        private void LongRangeState(ScannedRobotEvent inputEvent)
        {
            Evade(inputEvent);

            Fire(inputEvent);
        }

        private void CloseRangeState(ScannedRobotEvent inputEvent)
        {
            Evade(inputEvent);

            Charge(inputEvent);
        }

        private void EvadeState(ScannedRobotEvent inputEvent)
        {
            Evade(inputEvent);
        }

        private void ScannedRobot(Event inputEvent)
        {
            // Converting the input event to a ScannedRobotEvent
            ScannedRobotEvent convertedEvent = (ScannedRobotEvent)inputEvent;

            // If the scanned robot is one of my teammates
            if (pizzaBot.IsTeammate(convertedEvent.Name))
            {
                // Evade state
                Evade(convertedEvent);

                return;
            }

            // If the scanned robot isn't one of my teammates
            if (!pizzaBot.IsTeammate(convertedEvent.Name))
            {
                // Setting the current target to be the scanned robot
                currentTarget = convertedEvent.Name;

                // If we never scanned this enemy robot before
                if (!enemyRobots.ContainsKey(currentTarget))
                {
                    // Adding it to our dict of enemy robots
                    enemyRobots.Add(convertedEvent.Name, convertedEvent);
                }

                // If the pizzaBot's energy level is greater than/equal to 15
                if (pizzaBot.Energy >= 15)
                {
                    // Long range state
                    LongRangeState(convertedEvent);
                }
                // Else if the pizzaBot's energy level is less than 15
                else if (pizzaBot.Energy < 15)
                {
                    // Close range state
                    CloseRangeState(convertedEvent);
                }
            }
            else
            {
                // Evade state
                EvadeState(convertedEvent);
            }
        }

        private void Charge(ScannedRobotEvent inputEvent)
        {
            // Initializing the enemy bearing
            double enemyBearing = pizzaBot.HeadingRadians + inputEvent.BearingRadians;

            // Move towards the enemy
            pizzaBot.SetAhead(inputEvent.Distance * 0.8);

            // Turn towards the enemy
            pizzaBot.SetTurnRightRadians(Utils.NormalRelativeAngle(enemyBearing - pizzaBot.HeadingRadians));

            // Turn the gun and radar towards the enemy
            pizzaBot.SetTurnGunRightRadians(Utils.NormalRelativeAngle(enemyBearing - pizzaBot.GunHeadingRadians));
            pizzaBot.SetTurnRadarRightRadians(Utils.NormalRelativeAngle(enemyBearing - pizzaBot.RadarHeadingRadians));

            // Fire at the enemy
            pizzaBot.SetFire(Rules.MAX_BULLET_POWER);

            // Update the enemy robot information
            enemyRobots[inputEvent.Name] = inputEvent;

            // Execute the actions
            pizzaBot.Execute();
        }

        private void OnHitWall(Event inputEvent)
        {
            // Moving pizzaBot a 100 pixels backward
            pizzaBot.Ahead(100 * (robotDirection * -1));
        }

        private void OnHitByBullet(Event inputEvent)
        {
            // Converting the input event to a HitByBulletEvent
            HitByBulletEvent convertedEvent = (HitByBulletEvent)inputEvent;

            // If the enemy is not located
            if (!enemyLocated)
            {
                // Calculating the enemy bearing using the converted event's bearing radians + pizzaBot's heading radians
                double enemyBearing = convertedEvent.BearingRadians + pizzaBot.HeadingRadians;

                // Calculating the gun turn amount using the enemy's bearing + pizzaBot's gun heading radians
                double gunTurnAmount = Utils.NormalRelativeAngle(enemyBearing - pizzaBot.GunHeadingRadians);

                // Calculating the radar turn amount using the enemy's bearing + pizzaBot's radar heading radians
                double radarTurnAmount = Utils.NormalRelativeAngle(enemyBearing - pizzaBot.RadarHeadingRadians);

                // Setting pizzaBot's gun to turn right using the gun turn amount
                pizzaBot.SetTurnGunRightRadians(gunTurnAmount);

                // Setting pizzaBot's radar to turn right using the radar turn amount
                pizzaBot.SetTurnRadarRightRadians(radarTurnAmount);
            }
        }

        private void Evade(ScannedRobotEvent inputEvent)
        {
            // If the robot we scanned is our current target + their energy level decreased (they fired)
            if (inputEvent.Name.Equals(currentTarget) && inputEvent.Energy < enemyRobots[currentTarget].Energy)
            {
                // Going into a different direction
                robotDirection *= -1;
            }

            // Evading enemy fire by turning the robot
            pizzaBot.SetAhead(inputEvent.Distance * robotDirection);

            pizzaBot.SetTurnRightRadians(inputEvent.BearingRadians + Math.PI / 2);

            // Updating the enemy robot in our dict
            enemyRobots[inputEvent.Name] = inputEvent;
        }

        private void Fire(ScannedRobotEvent inputEvent)
        {
            // Initializing the bearing between them
            double theBearing = pizzaBot.HeadingRadians + inputEvent.BearingRadians;

            // Initializing the firing power of the pizzaBot
            double firingPower = 5;

            // Initializing the estimated X
            double estimatedPointX = pizzaBot.X + inputEvent.Distance * Math.Sin(theBearing);

            // Initializing the estimated Y
            double estimatedPointY = pizzaBot.Y + inputEvent.Distance * Math.Cos(theBearing);

            // Updating the estimated point
            for (int i = 1; i * 11 < CalculateDistance(pizzaBot.X, pizzaBot.Y, estimatedPointX, estimatedPointY); i++)
            {
                estimatedPointX += inputEvent.Velocity * Math.Sin(inputEvent.HeadingRadians);

                estimatedPointY += inputEvent.Velocity * Math.Cos(inputEvent.HeadingRadians);
            }

            // Calculating the heading of the target
            double targetsHeading = Math.Atan2(estimatedPointX - pizzaBot.X, estimatedPointY - pizzaBot.Y);

            // Calculating the angle to turn the gun
            double turnGunAngle = Utils.NormalRelativeAngle(targetsHeading - pizzaBot.GunHeadingRadians);

            // Turning the pizzaBot's gun to the right
            pizzaBot.SetTurnGunRightRadians(turnGunAngle);

            // Calculating the angle to turn the radar
            double turnRadarAngle = Utils.NormalRelativeAngle(theBearing - pizzaBot.RadarHeadingRadians);

            // Turning the pizzaBot's radar to the right
            pizzaBot.SetTurnRadarRightRadians(turnRadarAngle);

            // Having the pizzaBot fire with the specified firing power
            pizzaBot.SetFire(firingPower);

            pizzaBot.Execute();
        }

        public void Update()
        {
            // Turning the radar right to scan for enemies
            pizzaBot.TurnRadarRight(Double.PositiveInfinity);
        }

        private double CalculateDistance(double x1, double y1, double x2, double y2)
        {
            // Calculating distance x squared
            double dxSquared = (x2 - x1) * (x2 - x1);

            // Calculating distance y squared
            double dySquared = (y2 - y1) * (y2 - y1);

            // Returning the square root of their sum
            return Math.Sqrt(dxSquared + dySquared);
        }

        private void MessageReceived(Event inputEvent)
        {
            // Converting the input event to a MessageEvent
            MessageEvent convertedEvent = (MessageEvent)inputEvent;

            // If the number of enemies is less than the number of teammates, focus fire!
            if (enemyCount < teammateCount)
            {
                // Updating the current target to be the broadcasted enemy
                currentTarget = convertedEvent.Message.ToString();
            }

            Update();
        }

        private void RobotDied(Event inputEvent)
        {
            RobotDeathEvent deathVersion = (RobotDeathEvent)inputEvent;

            string robotName = deathVersion.Name.ToString();

            // If the dead robot is one of my teammates
            if (pizzaBot.IsTeammate(robotName))
            {
                teammateCount -= 1;
            }
            // Else if the dead robot is an enemy
            else if (!pizzaBot.IsTeammate(robotName))
            {
                enemyCount -= 1;
            }
        }
    }
}