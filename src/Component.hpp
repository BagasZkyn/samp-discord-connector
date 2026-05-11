#pragma once

#include "Singleton.hpp"
#include "types.hpp"

#include <string>
#include <vector>
#include <atomic>
#include <json.hpp>

using json = nlohmann::json;

struct SelectOption
{
	SelectOption(std::string const& label, std::string const& value,
		std::string const& description, std::string const& emoji_name,
		std::string const& emoji_id) :
		_label(label), _value(value), _description(description),
		_emoji_name(emoji_name), _emoji_id(emoji_id) {}

	std::string _label;
	std::string _value;
	std::string _description;
	std::string _emoji_name;
	std::string _emoji_id;
};

class Button
{
public:
	Button(std::string const& custom_id, std::string const& label, int style);
	~Button() = default;

	void SetLabel(std::string const& label) { _label = label; }
	void SetStyle(int style) { _style = style; }
	void SetEmoji(std::string const& name, std::string const& id)
	{
		_emoji_name = name;
		_emoji_id = id;
	}
	void SetUrl(std::string const& url) { _url = url; }
	void SetDisabled(bool disabled) { _disabled = disabled; }

	std::string const& GetCustomId() const { return _custom_id; }
	std::string const& GetLabel() const { return _label; }
	int GetStyle() const { return _style; }
	std::string const& GetUrl() const { return _url; }
	bool IsDisabled() const { return _disabled; }

	json DumpToJson() const;

private:
	std::string _custom_id;
	std::string _label;
	int _style;
	std::string _emoji_name;
	std::string _emoji_id;
	std::string _url;
	bool _disabled = false;
};

class SelectMenu
{
public:
	SelectMenu(std::string const& custom_id, std::string const& placeholder,
		int min_values, int max_values);
	~SelectMenu() = default;

	void AddOption(std::string const& label, std::string const& value,
		std::string const& description = "", std::string const& emoji_name = "",
		std::string const& emoji_id = "");
	void SetDisabled(bool disabled) { _disabled = disabled; }
	void SetPlaceholder(std::string const& placeholder) { _placeholder = placeholder; }
	void SetMinValues(int min_values) { _min_values = min_values; }
	void SetMaxValues(int max_values) { _max_values = max_values; }

	json DumpToJson() const;

private:
	std::string _custom_id;
	std::string _placeholder;
	int _min_values;
	int _max_values;
	bool _disabled = false;
	std::vector<SelectOption_t> _options;
};

class ActionRow
{
public:
	ActionRow() = default;
	~ActionRow() = default;

	void AddButton(ButtonId_t button);
	void AddSelectMenu(SelectMenuId_t menu);
	bool CanAddComponent() const;

	json DumpToJson() const;

	std::vector<ButtonId_t> const& GetButtons() const { return _buttons; }
	SelectMenuId_t GetSelectMenu() const { return _select_menu_id; }

private:
	std::vector<ButtonId_t> _buttons;
	SelectMenuId_t _select_menu_id = INVALID_SELECT_MENU_ID;
};

class ButtonManager : public Singleton<ButtonManager>
{
	friend class Singleton<ButtonManager>;
private:
	ButtonManager() = default;
	~ButtonManager() = default;

private:
	std::map<ButtonId_t, Button_t> m_Buttons;

public:
	ButtonId_t AddButton(std::string const& custom_id, std::string const& label, int style);
	bool DeleteButton(ButtonId_t id);
	Button_t const& FindButton(ButtonId_t id);
};

class SelectMenuManager : public Singleton<SelectMenuManager>
{
	friend class Singleton<SelectMenuManager>;
private:
	SelectMenuManager() = default;
	~SelectMenuManager() = default;

private:
	std::map<SelectMenuId_t, SelectMenu_t> m_Menus;

public:
	SelectMenuId_t AddSelectMenu(std::string const& custom_id,
		std::string const& placeholder, int min_values, int max_values);
	bool DeleteSelectMenu(SelectMenuId_t id);
	SelectMenu_t const& FindSelectMenu(SelectMenuId_t id);
	static json DumpOptionsToJson(std::vector<SelectOption_t> const& options);
};

class ActionRowManager : public Singleton<ActionRowManager>
{
	friend class Singleton<ActionRowManager>;
private:
	ActionRowManager() = default;
	~ActionRowManager() = default;

private:
	std::map<ActionRowId_t, ActionRow_t> m_Rows;

public:
	ActionRowId_t AddActionRow();
	bool DeleteActionRow(ActionRowId_t id);
	ActionRow_t const& FindActionRow(ActionRowId_t id);
	static json DumpComponentsToJson(std::vector<ActionRowId_t> const& rows);
};
