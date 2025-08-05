## events and signals
busy-waiting is for muscle-brained chooms. To build some seriously
scacable soft, we gotta have signals.

Jokes aside, this repo contains a simple low effort custom signal
implementation that allows you to `attach` callbacks and trigger 
those callbacks using `emit`. It creates an event driven paradigm
where things are happening as they are needed.

An example where this is useful is in game logic where you might
have an entity with component damagable which responds to a signal
"damaged" which informs the entity it took damage.

Whats the difference between an event and a signal? Implementation wise
not a lot, both are essentially an off-thread handler, or multiple, that
pick up small tasks from a task queue produced by a main-loop. Semantically
maybe events are unordered and signals are ordered; something like this.
