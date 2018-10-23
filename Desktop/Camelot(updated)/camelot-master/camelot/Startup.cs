using Microsoft.Owin;
using Owin;


[assembly: OwinStartupAttribute(typeof(Camelot.Startup))]
namespace Camelot
{
    public partial class Startup
    {


        public void Configuration(IAppBuilder app)
        {

            ConfigureAuth(app);// default

        }



    }
}
