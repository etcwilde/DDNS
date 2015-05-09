import hashlib


class ChordKey:
    def __init__(self, id="", key=None):
        if key is not None:
            self.key = key
        else:
            self.identifier = id
            self.key = hashlib.sha1(self.identifier.encode()).hexdigest()

    def __lt__(self, o):
        pass

    def __gt__(self, o):
        pass

    def __eq__(self, o):
        pass
