#ifndef KIND_H__
#define KIND_H__

#include <basics.h>

namespace Container {
  enum ContainerKind {
    None,
    Pan,
    Pot,
    Plate,
    DirtyPlates,
  };
  inline ContainerKind decode(std::string containerName) {
    if (containerName == "Plate")
      return Plate;
    if (containerName == "DirtyPlates")
      return DirtyPlates;
    if (containerName == "Pot")
      return Pot;
    if (containerName == "Pan")
      return Pan;
    // not a container
    return None;
  }
}

namespace Cooker {
  enum CookerKind {
    Chop,
    Pan,
    Pot,
    None
  };
  inline CookerKind decode(std::string cookerName) {
    if (cookerName == "-chop->")
      return Chop;
    if (cookerName == "-pan->")
      return Pan;
    if (cookerName == "-pot->")
      return Pot;
    assert(0);
    return None;
  }
}

namespace Tile {
  enum TileKind {
    None,
    Void,
    Floor,
    Wall,
    Table,
    IngredientBox,
    Trashbin,
    ChoppingStation,
    ServiceWindow,
    Stove,
    PlateReturn,
    Sink,
    PlateRack,
  };

  inline TileKind decode(char kindChar) {
    switch (kindChar) {
    case '_':
      return TileKind::Void;
    case '.':
      return TileKind::Floor;
    case '*':
      return TileKind::Table;
    case 't':
      return TileKind::Trashbin;
    case 'c':
      return TileKind::ChoppingStation;
    case '$':
      return TileKind::ServiceWindow;
    case 's':
      return TileKind::Stove;
    case 'p':
      return TileKind::PlateReturn;
    case 'k':
      return TileKind::Sink;
    case 'r':
      return TileKind::PlateRack;
    default:
      if (kindChar >= 'A' && kindChar <= 'Z')
        return TileKind::IngredientBox;
      Log("getchar %c", kindChar);
      assert(0);
    }
    assert(0);
    return TileKind::None;
  }
  inline char encode(TileKind kind) {
    switch (kind) {
      case TileKind::IngredientBox:
        return 'i';
      case TileKind::Trashbin:
        return 't';
      case TileKind::ChoppingStation:
        return 'c';
      case TileKind::ServiceWindow:
        return '$';
      case TileKind::Stove:
        return 's';
      case TileKind::PlateReturn:
        return 'p';
      case TileKind::Sink:
        return 'k';
      case TileKind::PlateRack:
        return 'r';
      case TileKind::Floor:
        return '.';
      case TileKind::Void:
        return '_';
      case TileKind::Table:
        return '*';
      default:
        assert(0);
    }
    assert(0);
    return '\0';
  }
}

#endif