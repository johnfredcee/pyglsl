#if !defined(SHADERNAMEVALIDATOR_H_INCLUDED)
#define SHADERNAMEVALIDATOR_H_INCLUDED

class ShaderNameValidator : public wxValidator
{
public:

    ShaderNameValidator( )
        : wxValidator()
    {
    }

    virtual wxObject* Clone() const
    {
        return new ShaderNameValidator;
    }

    virtual bool Validate(wxWindow* WXUNUSED(parent))
    {
		const ShaderListView* shaders = ShaderListView::Instance();
		
        wxTextCtrl* tc = wxDynamicCast(GetWindow(), wxTextCtrl);
        wxCHECK_MSG(tc, true, wxT("validator window must be wxTextCtrl"));

        wxString shaderPath = tc->GetValue();
		if (shaders->IsValidShader(shaderPath))
			return true;

        ::wxMessageBox(wxString::Format(wxT("%s is not a known shader"),m_invalidWord.c_str()),
                       wxT("Shader Validation Failure"));

        return false;
    }

private:
    wxString    m_invalidWord;
};

#endif



