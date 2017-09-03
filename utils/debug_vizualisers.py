#!/usr/bin/python
import gdb.printing


class DebugStructure:
    # TODO: improve via https://www.rethinkdb.com/blog/make-debugging-easier-with-custom-pretty-printers/
    def __init__(self, structure):
        self._structure = structure

    def to_string(self):
        return 'My representation of DebugStructure with key={} and next={}'.format(
            self._structure['key'],
            self._structure['next']
        )


def lookup_type(val):
    if str(val.type) == 'DebugStructure':
        return DebugStructure(val)
    return None


gdb.pretty_printers.append(lookup_type)
