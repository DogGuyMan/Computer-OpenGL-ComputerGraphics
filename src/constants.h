namespace Metahuman
{
	namespace TEXTURE
	{
		static const char *TEX_KERORO_FACE = "resources/textures/KeroroFace.png";
		static const char *TEX_KERORO_BODY = "resources/textures/KeroroBody.png";
		static const char *TEX_KERORO_HAT  = "resources/textures/KeroroHat_c.png";
	}

	namespace SCENE
	{
		static const char *const SCENE_SAVE_PATH = "resources/scene_state.json";
	}

	namespace MODEL
	{
		static const char *const KERORO_HEAD = "KeroroHead";
		static const char *const KERORO_BODY = "KeroroBody";
		static const char *const KERORO_HAT  = "KeroroHat";
		static const char *const UNKNOWN     = "Unknown";
		static const char *const MODEL_TYPES[] = {KERORO_HEAD, KERORO_BODY, KERORO_HAT};
	}

	namespace APP
	{
		static const char *const DEFAULT_TITLE = "Application";
		static const char *const WINDOW_TITLE  = "Example";
	}

	namespace UI
	{
		namespace PANEL
		{
			static const char *const TRANSFORM   = "Transform";
			static const char *const UV          = "UV";
			static const char *const PARAMETRIC  = "Parametric";
			static const char *const HYPERBOLOID = "Hyperboloid";
			static const char *const MODELS      = "Models";
			static const char *const SCENE       = "Scene";
		}
		namespace LABEL
		{
			static const char *const TARGET_MODEL       = "Target Model";
			static const char *const POSITION           = "Position";
			static const char *const ROTATE_DEG         = "Rotate(deg)";
			static const char *const SCALE              = "Scale";
			static const char *const UV_OFFSET          = "UV Offset";
			static const char *const UV_SCALE           = "UV Scale";
			static const char *const UV_ROTATE_DEG      = "UV Rotate(deg)";
			static const char *const RANGE              = "Range";
			static const char *const U_START            = "u Start";
			static const char *const U_END              = "u End";
			static const char *const V_START            = "v Start";
			static const char *const V_END              = "v End";
			static const char *const RESOLUTION         = "Resolution";
			static const char *const U_RES              = "u Res";
			static const char *const V_RES              = "v Res";
			static const char *const HYPERBOLOID_HEADER = "Hyperboloid (Paul Bourke)";
			static const char *const RADIUS             = "Radius";
			static const char *const HEIGHT             = "Height";
			static const char *const SHAPE              = "Shape (d)";
			static const char *const TYPE               = "Type";
			static const char *const ID                 = "ID";
			static const char *const ADD_MODEL          = "Add Model";
			static const char *const SAVE_JSON          = "Save JSON";
			static const char *const SAVE_COMPLETE      = "Save complete";
			static const char *const SAVE_FAILED        = "Save failed";
		}
	}
}; // namespace Metahuman
