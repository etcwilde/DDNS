#
# ChordNode.py
#
# Author: Evan Wilde		<etcwilde@uvic.ca>
# Date:   May 09 2015
#


class ChordNode:
    def __init__(self, nodeId, nodeKey, predecessor, sucessor, fingerTable):
        self.nodeId = nodeId
        self.nodeKey = nodeKey
        self.predecessor = predecessor
        self.sucessor = sucessor
        self.fingerTable = fingerTable

    # Look up a successor
    # if Id is none, uses key for lookup
    # if Key is none, uses id for lookup
    def getSuccessor(self, id=None, key=None):
        pass

    # Gets the closest preceeding node
    def _closesPrecedingNode(self, key):
        pass

    def create(self):
        self.predecessor = None
        self.sucessor = None

    def join(self, Node):
        self.predecessor = None
        self.sucessor = Node.getSuccessor(self.nodeId)

    # Verifies
    # Call periodically
    def stabilize(self):
        pass

    # Refresh Finger table
    def fixFingers(self):
        pass
