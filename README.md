## The Latinex mock exchange

This implements a multiple order book exchange with a FIX front end and ITCH datafeed

The FIX server runs on port 11001

The data feed runs on port  12001

The data feed (for now) is a raw ITCH feed

The idea is to mock what Nasdaq ME does for Latinex so as to make realistic simulations for testing and demonstrations

### Next steps

The next layer above should connect to this FIX interface for order entry operations, as well as to ITCH for keeping
track of ALL transactions on the exchange. This is the way to build a historical database as well as data feeds for
their clients.
