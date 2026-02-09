# Changelog
## [0.2.0] - 2026.02.09
### Added
 - `AttributeNotNamed` exception
 - `RecordItemNotDescribed` exception
 - `VariableNotDescribed` exception
 - `VariableRefHasNoActionMsg` exception
 - `RecordItem::tryDescription()` method
 - `Menu::tryName()` method
 - `Variable::tryDescription()` method
 - `VariableRef::tryDescription()` method
 - `VariableRef::tryActionMessage()` method
 - `NamedAttributePtr getValueName(const DataValue&, const SimpleDatatypeValue&, std::optional<uint8_t>)` 
 method implementation to use `std::holds_alternative` instead of `Variant_Visitor::match` 
 - `Datatype toDatatype(const DataValue&)` method implementation to use `std::holds_alternative` instead 
 of `Variant_Visitor::match`    

### Changed
 - pugixml to be a hidden dependency
 - pugixml to header only library
 - date to be a hidden dependency
 - date to header only library
 - date to use systems timezone database
 - variant_visitor to v0.2
 - variant_visitor to be a hidden dependency
 - `std::optional<TextID>` into `TextIDPtr`

### Removed
 - HSCUL dependency
 - `TextID::operator bool()` method

## [0.1.1] - 2025.06.02
### Changed 
 - gtest to v1.16
 - pugixml to v1.15

## [0.1.0] - 2025.02.12
 - initial release

## [Initial Commit] - 2024.04.30
