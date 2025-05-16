module example-module-actor

go 1.23.0

replace github.com/tigrux/traeger/go => ../../../go

replace actor_messaging => ../example-actor-messaging

require (
	actor_messaging v0.0.0-00010101000000-000000000000
	github.com/tigrux/traeger/go v0.0.0-20250420015205-1974910553d0
)
