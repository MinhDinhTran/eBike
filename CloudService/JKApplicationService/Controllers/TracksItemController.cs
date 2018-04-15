using System.Linq;
using System.Threading.Tasks;
using System.Web.Http;
using System.Web.Http.Controllers;
using System.Web.Http.OData;
using Microsoft.Azure.Mobile.Server;
using JKApplicationService.DataObjects;
using JKApplicationService.Models;

namespace JKApplicationService.Controllers
{
    public class TracksItemController : TableController<TracksItem>
    {
        protected override void Initialize(HttpControllerContext controllerContext)
        {
            base.Initialize(controllerContext);
            JKApplicationContext context = new JKApplicationContext();
            DomainManager = new EntityDomainManager<TracksItem>(context, Request);
        }

        // GET tables/TracksItem
        public IQueryable<TracksItem> GetAllTracksItems()
        {
            return Query();
        }

        // GET tables/TracksItem/
        public SingleResult<TracksItem> GetTracksItem(string id)
        {
            return Lookup(id);
        }

        // PATCH tables/Tracks
        public Task<TracksItem> PatchTracksItem(string id, Delta<TracksItem> patch)
        {
            return UpdateAsync(id, patch);
        }

        // POST tables/TracksItem
        public async Task<IHttpActionResult> PostTracksItem(TracksItem item)
        {
            TracksItem current = await InsertAsync(item);
            return CreatedAtRoute("Tables", new { id = current.Id }, current);
        }

        // DELETE tables/TracksItem/48D68C86-6EA6-4C25-AA33-223FC9A27959
        public Task DeleteTracksItem(string id)
        {
            return DeleteAsync(id);
        }
    }
}