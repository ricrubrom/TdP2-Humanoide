You are working on a web interface that has two sections: a **Control** tab and a **Kinematics** tab.

I want to **completely redesign the Kinematics tab** and turn it into a **step-by-step walking tutorial**. The Control tab must remain unchanged.

### New behavior for the Kinematics tab

* This tab is **tutorial-only**. The only control action available should be a single button or action called **`stepForward`**.
* Each `stepForward` action is composed of **two phases**:

  * In one phase, **one leg advances** while the **other holds**.
  * In the next phase, they **swap roles**.
* In **each phase**, there are **three servo writes** per leg (reuse the existing logic from the walking code).

### Tutorial logic

For the tutorial view, I want the following behavior for **each leg**:

* Before **each of the 4 total writes per leg** (as defined by the existing walking sequence), the UI should:

  1. Show the **three angles** calculated for that leg.
  2. Explain **how those angles were calculated** (using the kinematics logic already implemented).
  3. Then **execute that servo write**.
* The tutorial should clearly walk the user through **phase by phase** and **write by write**, so they can see:

  * Current leg (e.g., left/right, front/back if applicable).
  * The three joint angles.
  * A short explanation (or breakdown) of the calculation.
  * The moment when the servo command is actually sent.

### Code structure requirements

* **Reuse or adapt** the functions in `@walking.cpp` wherever possible, instead of rewriting kinematics or walking logic from scratch.
* Create a new file called **`@walking_tutorial.cpp`**:

  * Extract or wrap the necessary logic from `walking.cpp` so that:

    * It exposes clean functions to:

      * Compute the three angles for a given leg and phase.
      * Step through each servo write in order.
    * It can be called from the web interface layer to:

      * Get the angles and explanations.
      * Trigger each servo write in sequence.
* Update the Kinematics tab frontend so that it:

  * Calls into `walking_tutorial.cpp` for data and actions.
  * Presents the tutorial steps in a clear, sequential way (e.g., “Step 1/4 for this leg”, “Step 2/4”, etc).

### What to implement

1. Refactor or wrap logic in `@walking.cpp` as needed.
2. Implement `@walking_tutorial.cpp` with a clean API focused on:

   * Getting angles + explanation per leg/phase/write.
   * Executing the corresponding servo write.
3. Modify the Kinematics tab so that:

   * It only exposes the `stepForward` tutorial flow.
   * It shows the angles and their explanation right before each write.
   * It clearly sequences through the two phases and the 4 writes per leg.

Keep the existing Control tab behavior intact.
